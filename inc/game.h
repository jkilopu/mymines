#ifndef __GAME_H
#define __GAME_H

#include "map.h"

void setup(void);
void start(Map *mP, unsigned short col, unsigned short row, unsigned short n_mines);

void show_unknown(unsigned short col, unsigned short row);
bool click_map(Map map, short y, short x, bool *first_click);
void show_map(Map map, short y, short x);

void restart(Map *mP, unsigned short col, unsigned short row, unsigned short n_mines);
void wrapup(Map *mP);

#endif
