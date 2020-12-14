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
    if(!(p_timer->timer_id = SDL_AddTimer(INTERVAL, timer_callback, p_timer)))
        SDL_other_fatal_error("Can't set timer!\n%s\n", SDL_GetError());
}

/**
 * @brief Set the timer block pos.
 * 
 * @param p_timer The timer.
 */
void set_timer_pos(Timer *p_timer, unsigned short win_x, unsigned short win_y)
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
        Error("Timer not exist.");
    p_timer->time_passed = 0;
}

/**
 * @brief The fucntion is called when
 * 
 * @param interval The current timer interval.
 * @param param The param specified in "SDL_AddTimer".
 * 
 * @return The next timer interval.
 */
Uint32 timer_callback(Uint32 interval, void *param)
{
    Timer *p_timer = (Timer *) param;
    if (p_timer->time_passed > 99 * 60 + 60)
        return interval;
    draw_timer(p_timer);
    p_timer->time_passed++;
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
    unsigned short mins_lo = (p_timer->time_passed / 60) % 10;
    unsigned short secs_lo = (p_timer->time_passed % 60) % 10;
    unsigned short mins_hi = (p_timer->time_passed / 60) / 10;
    unsigned short secs_hi = (p_timer->time_passed % 60) / 10;
    unsigned short block_size = (TIME_REGION_WIDTH < p_timer->time_block_x ? TIME_REGION_WIDTH : p_timer->time_block_y) / 3;
    SDL_Rect dst_r = {p_timer->time_block_x, p_timer->time_block_y, block_size, block_size};
    draw(main_renderer, block_textures[mins_hi], NULL, &dst_r);
    dst_r.y += block_size + block_size / 4;
    draw(main_renderer, block_textures[mins_lo], NULL, &dst_r);
    dst_r.y += block_size + block_size / 3;
    draw(main_renderer, block_textures[secs_hi], NULL, &dst_r);
    dst_r.y += block_size + block_size / 4;
    draw(main_renderer, block_textures[secs_lo], NULL, &dst_r);
    SDL_RenderPresent(main_renderer);
}
