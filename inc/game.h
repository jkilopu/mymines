#ifndef __GAME_H
#define __GAME_H

#include "map.h"
#include <stdbool.h>

typedef struct _settings {
    unsigned short map_width, map_height;
    unsigned short n_mine;
    unsigned short window_height, window_width;
    unsigned short block_size;
} Settings;

void setup(void);
void start(Map *p_m);

void show_unknown(unsigned short col, unsigned short row);
void show_mines(Map map);
bool click_map(Map map, short y, short x, bool *first_click);
void show_block(Map map, short y, short x);
void set_draw_flag(Map map, unsigned short y, unsigned short x);
bool open_with_flag(Map map, unsigned short y, unsigned short x);
bool success(void);

void restart(Map *p_m);
void wrapup(Map *p_m);

#endif
