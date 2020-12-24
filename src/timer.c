/**
 * @file timer.c
 * @author jkilopu
 * @brief Provides functions for timing.
 */
#include "timer.h"
#include "SDL.h"
#include "render.h"
#include "fatal.h"

extern SDL_Renderer *main_renderer;
extern SDL_Texture *block_textures[];

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

/**
 * @brief Start timing.
 * 
 * @param p_timer The timer to set up.
 */
void set_timer(Timer *p_timer)
{
    p_timer->time_passed = 0;
    if(!(p_timer->timer_id = SDL_AddTimer(INTERVAL, timer_callback, &p_timer->time_passed)))
        SDL_other_fatal_error("Can't set timer!\n%s\n", SDL_GetError());
}

/**
 * @brief Set the timer block pos.
 * 
 * @param p_timer The timer.
 */
void set_timer_pos(Timer *p_timer, unsigned int win_x, unsigned int win_y)
{
    p_timer->time_block_x = win_x - TIME_REGION_WIDTH + TIME_REGION_WIDTH / 4;
    p_timer->time_block_y = win_y / 3;
}

/**
 * @brief Stop timming.
 * 
 * @param p_timer The timer.
 */
void unset_timer(Timer *p_timer)
{
    if (!SDL_RemoveTimer(p_timer->timer_id))
        Error("Timer not exist.\n");
    p_timer->time_passed = 0;
}

/**
 * @brief The fucntion is called when
 * 
 * @param interval The current timer interval.
 * @param param The param specified in "SDL_AddTimer".
 * 
 * @return The next timer interval.
 * 
 * @warning About multithread:
 * The timer in SDL is in a new thread, and SDL Renderer do NOT support multithread,
 * so do NOT use functions like "SDL_RenderPresent" in the callback function!!!
 */
Uint32 timer_callback(Uint32 interval, void *param)
{
    unsigned int *p_time_passed = param;
    if (*p_time_passed > 99 * 60 + 60)
        return interval;

    SDL_Event event;
    SDL_UserEvent user_event;

    user_event.type = SDL_USEREVENT;
    user_event.code = 0;
    user_event.data1 = p_time_passed;

    event.user = user_event;
    SDL_PushEvent(&event);
    
    return interval;
}

/**
 * @brief Draw the timer with the "main_renderer".
 * 
 * @param timer The timer to draw.
 * 
 * @warning This function can only be used after "set_timer" and "set_timer_pos" is called.`
*/
void draw_timer(Timer *p_timer)
{
    unsigned int mins_lo = (p_timer->time_passed / 60) % 10;
    unsigned int secs_lo = (p_timer->time_passed % 60) % 10;
    unsigned int mins_hi = (p_timer->time_passed / 60) / 10;
    unsigned int secs_hi = (p_timer->time_passed % 60) / 10;
    unsigned int block_size = (TIME_REGION_WIDTH < p_timer->time_block_x ? TIME_REGION_WIDTH : p_timer->time_block_y) / 3;
    SDL_Rect dst_r = {p_timer->time_block_x, p_timer->time_block_y, block_size, block_size};
    draw(main_renderer, block_textures[mins_hi], NULL, &dst_r);
    dst_r.y += block_size + block_size / 4;
    draw(main_renderer, block_textures[mins_lo], NULL, &dst_r);
    dst_r.y += block_size + block_size / 3;
    draw(main_renderer, block_textures[secs_hi], NULL, &dst_r);
    dst_r.y += block_size + block_size / 4;
    draw(main_renderer, block_textures[secs_lo], NULL, &dst_r);
}
