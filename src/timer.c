#include "timer.h"
#include "SDL.h"
#include "render.h"
#include "fatal.h"

static SDL_TimerID timer_id;
static unsigned short time_passed;
static unsigned short show_time_x;
static unsigned short show_time_y;
extern SDL_Renderer *main_renderer;
extern SDL_Texture *block_texture[];

void set_timer(void)
{
    if(!(timer_id = SDL_AddTimer(INTERVAL, timer_callback, NULL)))
        SDL_FatalError("Can't set timer!", SDL_GetError());
}

void unset_timer(void)
{
    if (!SDL_RemoveTimer(timer_id))
        Error("Timer not exist.");
    time_passed = 0;
}

Uint32 timer_callback(Uint32 interval, void *param)
{
    if (time_passed > 99 * 60 + 60)
        return interval;
    draw_timer();
    time_passed++;
    return interval;
}

void draw_timer(void)
{
    unsigned short mins_lo = (time_passed / 60) % 10;
    unsigned short secs_lo = (time_passed % 60) % 10;
    unsigned short mins_hi = (time_passed / 60) / 10;
    unsigned short secs_hi = (time_passed % 60) / 10;
    unsigned short block_size = (TIME_REGION_WIDTH < show_time_y ? TIME_REGION_WIDTH : show_time_y) / 3;
    SDL_Rect dst_r = {show_time_x, show_time_y, block_size, block_size};
    draw(main_renderer, block_texture[mins_hi], NULL, &dst_r);
    dst_r.y += block_size + block_size / 4;
    draw(main_renderer, block_texture[mins_lo], NULL, &dst_r);
    dst_r.y += block_size + block_size / 3;
    draw(main_renderer, block_texture[secs_hi], NULL, &dst_r);
    dst_r.y += block_size + block_size / 4;
    draw(main_renderer, block_texture[secs_lo], NULL, &dst_r);
    SDL_RenderPresent(main_renderer);
}

void set_timer_pos(unsigned short win_x, unsigned short win_y)
{
    show_time_x = win_x - TIME_REGION_WIDTH + TIME_REGION_WIDTH / 4;
    show_time_y = win_y / 3;
}
