/**
 * @file cursor.c
 * @author jkilopu
 * @brief Provide fuctions to draw cursor.
 */
#include "cursor.h"
#include "SDL.h"
#include "render.h"

extern SDL_Renderer *main_renderer;
SDL_Texture *remote_cursor_texture;

void draw_remote_cursor(unsigned int y, unsigned int x)
{
    if (y + CURSOR_HEIGHT > MAIN_WIN_SIZE || x + CURSOR_WIDTH > MAIN_WIN_SIZE)
        return;
    SDL_Rect r = { x, y, CURSOR_WIDTH, CURSOR_HEIGHT };
    draw(main_renderer, remote_cursor_texture, NULL, &r);
}
