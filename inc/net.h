/**
 * @file net.h
 * @author jkilopu
 * @brief Simple net module to help establish connection, send and receive packet between client and server.
 * 
 * @note
 * 1. Only support one client and one server at prsent.
 * 2. I don't know if this module can be used in big endian machine.
 * 3. DO NOT run program that has this module on real server, I don't know if it is safe.
 * 
 * @details The design of this net module:
 * 1. After sending the settings packet and key packet, during the playing process,
 *    when there is a need for interaction (Mouse move, click, game end),
 *    "PacketType" packet must be sent everytime before sending a packet with type in "PacketTypeEnum",
 *    so that the remote knows how to handle the next packet.
 * 2. All packet members have fixed size, so that the transmission of packets is platform independent.
 * 3. The "socket_set" in "net.c" file is used to implement asynchrony.
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
 * @brief Help generate totally same map on both side.
 */
typedef struct {
    Uint64 key;
    Uint8 key_size;
} SeedKeyPacket;

/**
 * @brief All packet types sent during game process.
 */
typedef enum {
    TYPE_NONE,
    TYPE_MAP_POS,
    TYPE_WINDOW_POS,
    TYPE_QUIT,
} PacketTypeEnum;

typedef Uint8 PacketType;

/**
 * @brief Record pos of one click or mouse move.
 */
typedef struct {
    Uint16 pos_y;
    Uint16 pos_x;
} PositionPacket;

/**
 * @brief All click packet types.
 */
typedef enum {
    LEFT_CLICK,
    RIGHT_CLICK,
} ClickTypeEnum;

typedef Uint8 ClickType;

typedef struct {
    PositionPacket position_packet;
    ClickType click_type;
} ClickMapPacket;

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

static void server_resolve_host(IPaddress *p_addr, Uint16 port);
static void clinent_resolve_host(IPaddress *p_addr, const char *host, Uint16 port);
static SDL_bool is_connected_socket_ready(void);

void send_packet_type(PacketType packet_type);
PacketType recv_packet_type(void);
void send_click_map_packet(ClickType click_type, unsigned short y, unsigned short x);
ClickType recv_click_map_packet(unsigned short *p_y, unsigned short *p_x);

void host_game(Uint16 port, SeedKeyPacket *p_seed_key_packet, Settings *p_settings);
void join_game(const char *host, Uint16 port, SeedKeyPacket *p_seed_key_packet, Settings *p_settings);

void finish_sdl_net(void);

#endif
