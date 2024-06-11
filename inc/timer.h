/**
 * @file timer.h
 * @author jkilopu
 * @brief Stopwatch in mymines.
 */
#ifndef __TIMER_H
#define __TIMER_H

#include "SDL.h"
#include "render.h"

#define TIME_INTERVAL (1000)
#define TIME_REGION_WIDTH (MAIN_WIN_SIZE / 6)

//-------------------------------------------------------------------
// Type Definations
//------------------------------------------------------------------

/**
 * @brief Defination of mymines timer, with position and SDL2 timer.
 */
typedef struct _timer {
    SDL_TimerID timer_id;
    unsigned int time_passed; ///< Time in seconds
    SDL_Rect timer_block;
} Timer;

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void set_timer(Timer *p_timer);
void set_timer_pos(Timer *p_timer, unsigned int win_width, unsigned int win_height);
void unset_timer(Timer *p_timer);
void draw_timer(Timer *p_timer);
Uint32 timer_callback(Uint32 interval, void *param);

#endif
