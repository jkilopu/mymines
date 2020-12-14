#include "net.h"
#include "SDL_net.h"
#include "game.h"
#include "SDL_stdinc.h"
#include "fatal.h"

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
static void server_resolve_host(IPaddress *p_addr, Uint16 port)
{
    if (SDLNet_ResolveHost(p_addr, NULL, port) < 0)
    {
        SDL_net_error("Can't resolve server host!\n%s\n", SDL_GetError());
        exit(1);
    }    
}

/**
 * @brief Wrapper for "SDLNet_ResolveHost" used in client.
 * 
 * @param p_addr Points to the IPaddress that will be filled in.
 * @param host A hostname or IP to connect with the server.
 * @param port The server's listening port number.
 */
static void clinent_resolve_host(IPaddress *p_addr, const char *host, Uint16 port)
{
    if (SDLNet_ResolveHost(p_addr, host, port) < 0)
    {
        SDL_net_error("Can't resolve client host!\n%s\n", SDL_GetError());
        exit(1);
    }    
}

/**
 * @brief See if "connected_socket" in the "socket_set" in this module has the data ready.
 * 
 * @return Return SDL_TRUE if the "connected_socket" has the data ready.
 */
static SDL_bool is_connected_socket_ready(void)
{
    int ready_socket_num = SDLNet_CheckSockets(socket_set, 0);
    ///< Since "connected_socket" is the only socket in "socket_set", no need to call "SDLNet_SocketReady".

    if (ready_socket_num < 0)
        SDL_net_error("Check socket sets failed!\n%s\n", SDL_GetError());
    if (ready_socket_num == 0)
        return SDL_FALSE;
    return SDL_TRUE;
}

/**
 * @brief Send a packet type packet to "connected_socket".
 * 
 * @param packet_type The type of the packet, see the header file for details.
 * 
 * @note When to use "send_packet_type":
 * After sending the settings packet and key packet, during the playing process,
 * when there is a need for interaction (Mouse move, click, game end),
 * this function must be called so that if there is a packet follow it,
 * the remote knows how to handle the next packet.
 */
void send_packet_type(PacketType packet_type)
{
    if (SDLNet_TCP_Send(connected_socket, &packet_type, sizeof(PacketType)) != sizeof(PacketType))
        SDL_net_error("Send packet type not match!\n%s\n", SDL_GetError());
}

/**
 * @brief Receive a pakcet type packet from "connected_socket".
 * 
 * @return Packet type received
 */
PacketType recv_packet_type(void)
{
    PacketType packet_type = TYPE_NONE;
    if (is_connected_socket_ready())
        if (SDLNet_TCP_Recv(connected_socket, &packet_type, sizeof(PacketType)) != sizeof(PacketType))
            SDL_net_error("Recv packet_type failed!\n%s\n", SDL_GetError());
    return packet_type;
}

/**
 * @brief Send a click map packet.
 * 
 * @param click_type The click type, see header file for details.
 * @param y   The column number of the click.
 * @param x   The row number of the click.
 */
void send_click_map_packet(ClickType click_type, unsigned short y, unsigned short x)
{
    send_packet_type(TYPE_MAP_POS);

    ClickMapPacket click_map_packet;
    click_map_packet.click_type = click_type;
    click_map_packet.position_packet.pos_y = (Uint16) y;
    click_map_packet.position_packet.pos_x = (Uint16) x;
    if (SDLNet_TCP_Send(connected_socket, &click_map_packet, sizeof(ClickMapPacket)) != sizeof(ClickMapPacket))
        SDL_net_error("Send position packet length not match!\n%s\n", SDL_GetError());
}

/**
 * @brief Receive the click map packet.
 * 
 * @param p_y   Points to column number will be filled in.
 * @param p_x   Points to row number will be filled in.
 * 
 * @return The type of the click packet.
 * 
 * @warning When should the function be called:
 * This function must be called after "recv_packet_type" is called,
 * and the packet type is "TYPE_MAP_POS".
 */
ClickType recv_click_map_packet(unsigned short *p_y, unsigned short *p_x)
{
    ClickMapPacket click_map_packet;
    if (SDLNet_TCP_Recv(connected_socket, &click_map_packet, sizeof(ClickMapPacket)) != sizeof(ClickMapPacket))
        SDL_net_error("Recv position packet lenght not match!\n%s\n", SDL_GetError());
    *p_y = click_map_packet.position_packet.pos_y;
    *p_x = click_map_packet.position_packet.pos_x;
    return click_map_packet.click_type;
}

/**
 * @brief Listen on port and wait for connection, send the seed key and settings, add socket to socket set.
 * 
 * @param port The port to listen on.
 * @param p_seed_key_packet Point to the seed key packet will be sent.
 * @param p_settings Point to the settings will be sent.
 * 
 * @warning If the game is the server,
 * the function must be called before ANY send and recv function in this source file.
 */
void host_game(Uint16 port, SeedKeyPacket *p_seed_key_packet, Settings *p_settings)
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

    /** TODO: Show the client ip */
    IPaddress *peer_addr = SDLNet_TCP_GetPeerAddress(connected_socket);
    if (peer_addr == NULL)
        SDL_net_error("Can't get peer addr!\n");
    const char *peer_name = SDLNet_ResolveIP(peer_addr);
    if (peer_name == NULL)
        SDL_net_error("Can't resovle peer addr!\n");
    SDL_Log("Remode addr: %s\n", peer_addr);

    if (SDLNet_TCP_Send(connected_socket, p_seed_key_packet, sizeof(SeedKeyPacket)) != sizeof(SeedKeyPacket))
        SDL_net_error("Send seed_key_packet length not match!\n%s\n", SDL_GetError());
    if (SDLNet_TCP_Send(connected_socket, p_settings, sizeof(Settings)) != sizeof(Settings))
        SDL_net_error("Send settings length not match!\n%s\n", SDL_GetError());

    socket_set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(socket_set, connected_socket);
}

/**
 * @brief Try to connect remote port, receive the seed packet and settings, add socket to socket set.
 * 
 * @param host A hostname or IP to connect with the server.
 * @param port The server's listening port number.
 * @param p_seed_key_packet Points to seed key packet will be fileed in.
 * @param p_settings Points to settings will be fileed in.
 * 
 * @warning If the game is the client,
 * the function must be called before ANY send and recv function in this source file.
 */
void join_game(const char *host, Uint16 port, SeedKeyPacket *p_seed_key_packet, Settings *p_settings)
{
    IPaddress server_addr;
    clinent_resolve_host(&server_addr, host, port);

    /** TODO: 1. Draw Connect status 
     *        2. Allow user to quit.
     */
    while(!connected_socket)
        connected_socket = SDLNet_TCP_Open(&server_addr);

    if (SDLNet_TCP_Recv(connected_socket, p_seed_key_packet, sizeof(SeedKeyPacket)) != sizeof(SeedKeyPacket))
        SDL_net_error("Recv seed_key_packet length not match!\n%s\n", SDL_GetError());
    if (SDLNet_TCP_Recv(connected_socket, p_settings, sizeof(Settings)) != sizeof(Settings))
        SDL_net_error("Recv settings length not match!\n%s\n", SDL_GetError());
    
    SDL_Log("key: %llu, key_size: %hhu", p_seed_key_packet->key, p_seed_key_packet->key_size);
    
    socket_set = SDLNet_AllocSocketSet(1);
    SDLNet_TCP_AddSocket(socket_set, connected_socket);
}

/**
 * @brief Finish connection and SDL_Net.
 */
void finish_sdl_net(void)
{
    SDLNet_TCP_Close(connected_socket);
    SDLNet_FreeSocketSet(socket_set);
    SDLNet_Quit();
}
