#ifndef __TIMER_H
#define __TIMER_H

#include "SDL.h"

#define INTERVAL (1000)
#define TIME_REGION_WIDTH (100)

//-------------------------------------------------------------------
// Type Definations
//------------------------------------------------------------------

/**
 * @brief Defination of mymines timer, with position and SDL2 timer.
 */
typedef struct _timer {
    SDL_TimerID timer_id;
    unsigned short time_passed; ///< Time in seconds
    unsigned short time_block_x;
    unsigned short time_block_y;
} Timer;

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void set_timer(Timer *p_timer);
void set_timer_pos(Timer *p_timer, unsigned short win_x, unsigned short win_y);
void unset_timer(Timer *p_timer);
void draw_timer(Timer *p_timer);
Uint32 timer_callback(Uint32 interval, void *param);

#endif
