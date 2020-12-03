/**
 * @file game.h
 * @author jkilopu
 * @brief The core elements of Mines.
 */

#ifndef __GAME_H
#define __GAME_H

#include "map.h"
#include "timer.h"
#include <stdbool.h>

//-------------------------------------------------------------------
// Type Definations
//-------------------------------------------------------------------

/**
 * @brief All settings in mymines.
 */
typedef struct _settings {
    unsigned short map_width, map_height;
    unsigned short n_mine;
    unsigned short window_height, window_width;
    unsigned short block_size;
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
// Prototypes
//-------------------------------------------------------------------

Game setup(void);
void create_map_in_game(Game game);

bool click_map(Game game, short y, short x, bool *first_click);
void set_draw_flag(Game game, unsigned short y, unsigned short x);
static void show_blocks(Game game, short y, short x);
static void show_unknown(Map map);
static void show_mines(Map map);
static bool open_with_flag(Game game, unsigned short y, unsigned short x);

bool success(Game game);
static void destroy_game(Game game);
void finish(Game game);
void restart(Game game);
void wrapup(Game game);

#endif
