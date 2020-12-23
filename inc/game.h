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
    Uint32 map_width, map_height;
    Uint32 n_mine;
    Uint32 window_height, window_width;
    Uint32 block_size;
    Uint8 game_mode;
    Uint8 padding[3];
} Settings;
SDL_COMPILE_TIME_ASSERT(Settings, sizeof(Settings) == 28);

/**
 * @brief All elements in game.
 */
typedef struct _game {
    Map map;
    Settings settings;
    Timer timer;
    unsigned int opened_blocks;
    SDL_bool is_first_click;
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

SDL_bool handle_recved_packet(Game game);
SDL_bool click_map(Game game, unsigned int y, unsigned int x);
static void show_block_in_map_without_mine(Map map, unsigned int y, unsigned int x);
static void show_whole_map(Map map);
static void show_block_in_cursor(Map map, unsigned int cursor_y, unsigned int cursor_x);
void set_draw_flag(Game game, unsigned int y, unsigned int x);
static void show_blocks(Game game, unsigned int y, unsigned int x);
static SDL_bool open_with_flag(Game game, unsigned int y, unsigned int x);

SDL_bool success(Game game);
static void destroy_game(Game game);
void finish(Game game);
void restart(Game game);
void wrapup(Game game);

#endif
