/**
 * @file net.h
 * @author jkilopu
 * @brief Simple net module to help establish connection, send and receive packet between client and server.
 * 
 * @note
 * 1. Only support one client and one server at present.
 * 2. I don't know if this module can be used in big endian machine.
 * 3. DO NOT run program that has this module on real server, I don't know if it is safe.
 * 
 * @details The design of this net module(version 2):
 * 1. All packets sent and received in mymines are "MyMinesPacket", which is a union that has, hopefully,
 *    fixed size (32 bytes) in all platforms and compilers. (Inspired by "SDL2_Event")
 * 2. The type of the packet is at the header of the packet. Each type corresponds to a structure.
 *    With the help of type, the program knows how to deal with it. (So no "PacketType" packet in this version)
 * 3. The "socket_set" in "net.c" file is used to implement asynchrony.
 * 4. Use magic macro in SDL2 to ensure that the size of struct and union is fixed.
 */

#ifndef __NET_H
#define __NET_H

#include "SDL_net.h"
#include "SDL_stdinc.h"
#include "game.h"

//-------------------------------------------------------------------
// Type Definations
//-------------------------------------------------------------------

/**
 * @brief All packet types sent during game process.
 */
typedef enum {
    TYPE_NONE,
    TYPE_SEED_KEY,
    TYPE_SETTINGS,
    TYPE_CLICK_MAP,
    TYPE_MOUSE_MOVE,
    TYPE_QUIT,
} PacketTypeEnum;

typedef Uint8 PacketType;

/**
 * @brief Record the key and key_size to seed.
 * 
 * @note Use:
 * Help generate totally same map on both side.
 */
typedef struct {
    PacketType type;
    Uint8 key_size;
    Uint8 paddings[6];
    Uint64 key;
} SeedKeyPacket;
SDL_COMPILE_TIME_ASSERT(SeedKeyPacket, sizeof(SeedKeyPacket) == 16);

/**
 * @brief Record pos of mouse motion.
 */
typedef struct {
    PacketType type;
    Uint8 padding[3];
    Uint32 pos_y;
    Uint32 pos_x;
} MouseMovePacket;
SDL_COMPILE_TIME_ASSERT(MouseMovePacket, sizeof(MouseMovePacket) == 12);

/**
 * @brief All click packet types.
 */
typedef enum {
    LEFT_CLICK,
    RIGHT_CLICK,
} ClickTypeEnum;

typedef Uint8 ClickType;

/**
 * @brief Record the type and pos of mouse click.
 */
typedef struct {
    PacketType type;
    ClickType click_type;
    Uint8 paddings[2];
    Uint32 pos_y;
    Uint32 pos_x;
} ClickMapPacket;
SDL_COMPILE_TIME_ASSERT(ClickMapPacket, sizeof(ClickMapPacket) == 12);

/**
 * @brief Record game settings.
 */
typedef struct {
    PacketType type;
    Uint8 padding[3];
    Settings settings;
} SettingsPacket;
SDL_COMPILE_TIME_ASSERT(SettingsPacket, sizeof(SettingsPacket) == 32);

/**
 * @brief General packet union in mymines.
 * 
 * @note Always use "MyMinesPacket" to send and receive packet.
 */
typedef union {
    PacketType type;
    SettingsPacket settings_packet;
    SeedKeyPacket seed_key_packet;
    MouseMovePacket mouse_move_packet;
    ClickMapPacket click_map_packet;
    Uint8 padding[32];
} MyMinesPacket;
SDL_COMPILE_TIME_ASSERT(MyMinesPacket, sizeof(MyMinesPacket) == 32);

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

static void server_resolve_host(IPaddress *p_addr, Uint32 port);
static void clinent_resolve_host(IPaddress *p_addr, const char *host, Uint32 port);
SDL_bool is_connected_socket_ready(void);

static void fill_seed_key_packet(SeedKeyPacket *p_seed_key_packet, Uint64 key, Uint8 key_size);
static void fill_settings_packet(SettingsPacket *p_settings_packet, Settings *p_settings);
static void fill_click_map_packet(ClickMapPacket *p_click_map_packet,
        ClickType click_type, unsigned int y, unsigned int x);
static void fill_mouse_move_packet(MouseMovePacket *p_mouse_move_packet, unsigned int y, unsigned int x);

static void send_mymines_packet(MyMinesPacket *p_mymines_packet);
void send_seed_key_packet(Uint64 key, Uint8 key_size);
void send_settings_packet(Settings *p_settings);
void send_click_map_packet(ClickType click_type, unsigned int y, unsigned int x);
void send_mouse_move_packet(unsigned int y, unsigned int x);
void send_quit_packet(void);

void recv_mymines_packet(MyMinesPacket *p_mymines_packet);

void host_game(Uint32 port, Uint64 key, Uint8 key_size, Settings *p_settings);
void join_game(const char *host, Uint32 port, Uint64 *p_key, Uint8 *p_key_size, Settings *p_settings);

void finish_sdl_net(void);

#endif
