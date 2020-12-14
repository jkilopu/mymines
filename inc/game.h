/**
 * @file game.h
 * @author jkilopu
 * @brief The core elements of Mines.
 */

#ifndef __GAME_H
#define __GAME_H

#include "map.h"
#include "timer.h"
#include "SDL_stdinc.h"

//-------------------------------------------------------------------
// Type Definations
//-------------------------------------------------------------------

/**
 * @brief All settings in mymines.
 */
typedef struct {
    Uint16 map_width, map_height;
    Uint16 n_mine;
    Uint16 window_height, window_width;
    Uint16 block_size;
    Uint8 game_mode;
} Settings;

/**
 * @brief All elements in game.
 */
typedef struct _game {
    Map map;
    Settings settings;
    Timer timer;
    unsigned short opened_blocks;
} * Game;

//-------------------------------------------------------------------
// Game Mode Macros
//-------------------------------------------------------------------

#define LAN_LOCAL_BIT 0
#define SERVER_CLIENT_BIT 1
#define set_lan_mode(game_mode) (game_mode |= (1 << LAN_LOCAL_BIT))
#define set_local_mode(game_mode) (game_mode &= ~(1 << LAN_LOCAL_BIT))
#define set_server_mode(game_mode) (game_mode |= (1 << SERVER_CLIENT_BIT))
#define set_client_mode(game_mode) (game_mode &= ~(1 << SERVER_CLIENT_BIT))
#define is_lan_mode(game_mode) (game_mode & (1 << LAN_LOCAL_BIT))
#define is_server_mode(game_mode) (game_mode & (1 << SERVER_CLIENT_BIT))

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

Game setup(int argc);
static Game create_game_with_mode(Uint8 game_mode);
void connect_and_complete_setup(Game game);
void create_map_in_game(Game game);

SDL_bool handle_recved_packet(Game game, SDL_bool *first_click);
SDL_bool click_map(Game game, unsigned short y, unsigned short x, SDL_bool *first_click);
void set_draw_flag(Game game, unsigned short y, unsigned short x);
static void show_blocks(Game game, unsigned short y, unsigned short x);
static void show_unknown(Map map);
static void show_mines(Map map);
static SDL_bool open_with_flag(Game game, unsigned short y, unsigned short x);

SDL_bool success(Game game);
static void destroy_game(Game game);
void finish(Game game);
void restart(Game game);
void wrapup(Game game);

#endif
