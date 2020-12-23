/**
 * @file net.c
 * @author jkilopu
 * @brief Provides functions for connection and packet manipulation.
 */
#include "net.h"
#include "SDL_net.h"
#include "game.h"
#include "SDL_stdinc.h"
#include "fatal.h"
#include "SDL_log.h"
#include <stdarg.h>
#include <string.h>

static TCPsocket connected_socket;
static SDLNet_SocketSet socket_set;

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

/**
 * @brief Wrapper for "SDLNet_ResolveHost" used in server.
 * 
 * @param p_addr Points to the IPaddress that will be filled in.
 * @param port The port to listen on.
 */
static void server_resolve_host(IPaddress *p_addr, Uint32 port)
{
    if (SDLNet_ResolveHost(p_addr, NULL, port) < 0)
        SDL_net_error("Can't resolve server host: port: %hu!\n%s\n", port, SDL_GetError());
}

/**
 * @brief Wrapper for "SDLNet_ResolveHost" used in client.
 * 
 * @param p_addr Points to the IPaddress that will be filled in.
 * @param host A hostname or IP to connect with the server.
 * @param port The server's listening port number.
 */
static void clinent_resolve_host(IPaddress *p_addr, const char *host, Uint32 port)
{
    if (SDLNet_ResolveHost(p_addr, host, port) < 0)
        SDL_net_error("Can't resolve client host: %s:%hu!\n%s\n", host, port,SDL_GetError());
}

/**
 * @brief See if "connected_socket" in the "socket_set" in this module has the data ready.
 * 
 * @return Return SDL_TRUE if the "connected_socket" has the data ready.
 */
SDL_bool is_connected_socket_ready(void)
{
    int ready_socket_num = SDLNet_CheckSockets(socket_set, 0);
    ///< Since "connected_socket" is the only socket in "socket_set", no need to call "SDLNet_SocketReady".

    if (ready_socket_num < 0)
        SDL_net_error("Check socket sets failed!\n%s\n", SDL_GetError());
    if (ready_socket_num == 0)
        return SDL_FALSE;
    return SDL_TRUE;
}

//-------------------------------------------------------------------
// Fill packet
//-------------------------------------------------------------------

static void fill_seed_key_packet(SeedKeyPacket *p_seed_key_packet, Uint64 key, Uint8 key_size)
{
    p_seed_key_packet->type = TYPE_SEED_KEY;
    p_seed_key_packet->key = key;
    p_seed_key_packet->key_size = key_size;
}

static void fill_settings_packet(SettingsPacket *p_settings_packet, Settings *p_settings)
{
    p_settings_packet->type = TYPE_SETTINGS;
    p_settings_packet->settings.block_size = p_settings->block_size;
    p_settings_packet->settings.game_mode = p_settings->game_mode;
    p_settings_packet->settings.map_height = p_settings->map_height;
    p_settings_packet->settings.map_width = p_settings->map_width;
    p_settings_packet->settings.window_height = p_settings->window_height;
    p_settings_packet->settings.window_width = p_settings->window_width;
    p_settings_packet->settings.n_mine = p_settings->n_mine;
}

static void fill_click_map_packet(ClickMapPacket *p_click_map_packet,
        ClickType click_type, unsigned int y, unsigned int x)
{
    p_click_map_packet->type = TYPE_CLICK_MAP;
    p_click_map_packet->click_type = click_type;
    p_click_map_packet->pos_y = y;
    p_click_map_packet->pos_x = x;
}

static void fill_mouse_move_packet(MouseMovePacket *p_mouse_move_packet, unsigned int y, unsigned int x)
{
    p_mouse_move_packet->type = TYPE_MOUSE_MOVE;
    p_mouse_move_packet->pos_y = y;
    p_mouse_move_packet->pos_x = x;
}

//-------------------------------------------------------------------
// Send packet
//-------------------------------------------------------------------

static void send_mymines_packet(MyMinesPacket *p_mymines_packet)
{
    if (SDLNet_TCP_Send(connected_socket, p_mymines_packet, sizeof(MyMinesPacket)) != sizeof(MyMinesPacket))
        SDL_net_error("Send mymines packet len not match!\n%s\n", SDL_GetError());
}

void send_seed_key_packet(Uint64 key, Uint8 key_size)
{
    MyMinesPacket mymines_packet;
    fill_seed_key_packet(&mymines_packet.seed_key_packet, key, key_size);
    send_mymines_packet(&mymines_packet);
}

void send_settings_packet(Settings *p_settings)
{
    MyMinesPacket mymines_packet;
    fill_settings_packet(&mymines_packet.settings_packet, p_settings);
    send_mymines_packet(&mymines_packet);
}

void send_click_map_packet(ClickType click_type, unsigned int y, unsigned int x)
{
    MyMinesPacket mymines_packet;
    fill_click_map_packet(&mymines_packet.click_map_packet, click_type, y, x);
    send_mymines_packet(&mymines_packet);
}

void send_mouse_move_packet(unsigned int y, unsigned int x)
{
    MyMinesPacket mymines_packet;
    fill_mouse_move_packet(&mymines_packet.mouse_move_packet, y, x);
    send_mymines_packet(&mymines_packet);
}

void send_quit_packet(void)
{
    MyMinesPacket mymines_packet;
    mymines_packet.type = TYPE_QUIT;
    send_mymines_packet(&mymines_packet);
}

//-------------------------------------------------------------------
// Receive MyMinesPacket packet
//-------------------------------------------------------------------

void recv_mymines_packet(MyMinesPacket *p_mymines_packet)
{
    if (SDLNet_TCP_Recv(connected_socket, p_mymines_packet, sizeof(MyMinesPacket)) != sizeof(MyMinesPacket))
        SDL_net_error("Recv mymines packet len not match!\n%s\n", SDL_GetError());
}

//-------------------------------------------------------------------
// Launch as server or client
//-------------------------------------------------------------------

/**
 * @brief Listen on port and wait for connection, send the seed key and settings, add socket to socket set.
 * 
 * @param port The port to listen on.
 * @param key The seed key will be sent.
 * @param key_size The size of key will be sent.
 * @param p_settings Point to the settings will be sent.
 * 
 * @warning If the game is the server,
 * the function must be called before ANY send and recv function.
 */
void host_game(Uint32 port, Uint64 key, Uint8 key_size, Settings *p_settings)
{
    IPaddress listen_addr;
    server_resolve_host(&listen_addr, port);

    TCPsocket server_listen_socket = SDLNet_TCP_Open(&listen_addr);
    if (server_listen_socket == NULL)
        SDL_net_error("Can't open server listen socket!\n%s\n", SDL_GetError());

    /** TODO: 1. Draw listen status 
     *        2. Allow user to quit.
     */
    while (!connected_socket)
        connected_socket = SDLNet_TCP_Accept(server_listen_socket);

    SDLNet_TCP_Close(server_listen_socket);

    /** TODO: Show the client ip on window */
    IPaddress *peer_addr = SDLNet_TCP_GetPeerAddress(connected_socket);
    if (peer_addr == NULL)
        SDL_net_error("Can't get peer addr!\n");
    const char *peer_name = SDLNet_ResolveIP(peer_addr);
    if (peer_name == NULL)
        SDL_net_error("Can't resovle peer addr!\n");
    SDL_Log("Remode addr: %s\n", peer_name);

    send_seed_key_packet(key, key_size);
    send_settings_packet(p_settings);

    socket_set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(socket_set, connected_socket);
}

/**
 * @brief Try to connect remote port, receive the seed packet and settings, add socket to socket set.
 * 
 * @param host A hostname or IP to connect with the server.
 * @param port The server's listening port number.
 * @param p_key Points to key will be filled in.
 * @param p_key_size Points to key size will be filled in.
 * @param p_settings Points to settings will be filled in.
 * 
 * @warning If the game is the client,
 * the function must be called before ANY send and recv function.
 */
void join_game(const char *host, Uint32 port, Uint64 *p_key, Uint8 *p_key_size, Settings *p_settings)
{
    IPaddress server_addr;
    clinent_resolve_host(&server_addr, host, port);

    /** TODO: 1. Draw Connect status 
     *        2. Allow user to quit.
     */
    while(!connected_socket)
        connected_socket = SDLNet_TCP_Open(&server_addr);

    /** TODO: Show the server ip on window */
    IPaddress *peer_addr = SDLNet_TCP_GetPeerAddress(connected_socket);
    if (peer_addr == NULL)
        SDL_net_error("Can't get peer addr!\n");
    const char *peer_name = SDLNet_ResolveIP(peer_addr);
    if (peer_name == NULL)
        SDL_net_error("Can't resovle peer addr!\n");
    SDL_Log("Remode addr: %s\n", peer_name);

    socket_set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(socket_set, connected_socket);

    MyMinesPacket mymines_packet;
    while (!is_connected_socket_ready())
        ;
    recv_mymines_packet(&mymines_packet);
    if (mymines_packet.type != TYPE_SEED_KEY)
        Error("Packet should be a TYPE_SEED_KEY packet, not %hhu!\n", mymines_packet.type);
    *p_key = mymines_packet.seed_key_packet.key;
    *p_key_size = mymines_packet.seed_key_packet.key_size;
    SDL_Log("key: %llu, key_size: %hhu", *p_key, *p_key_size);

    while (!is_connected_socket_ready())
        ;
    recv_mymines_packet(&mymines_packet);
    if (mymines_packet.type != TYPE_SETTINGS)
        Error("Packet should be a TYPE_SETTINGS packet, not %hhu!\n", mymines_packet.type);
    memcpy(p_settings, &mymines_packet.settings_packet.settings, sizeof(Settings));
}

//-------------------------------------------------------------------
// Finish stuff
//-------------------------------------------------------------------

/**
 * @brief Finish connection and SDL_Net.
 */
void finish_sdl_net(void)
{
    SDLNet_TCP_Close(connected_socket);
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}
