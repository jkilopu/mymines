#ifndef __TIMER_H
#define __TIMER_H

#include "SDL.h"

#define INTERVAL (1000)
#define TIME_REGION_WIDTH (100)

void set_timer(void);
void unset_timer(void);
Uint32 timer_callback(Uint32 interval, void *param);
void draw_timer(void);
void set_timer_pos(unsigned short win_x, unsigned short win_y);

#endif