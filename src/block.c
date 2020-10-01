#include "block.h"
#include "render.h"

static unsigned short block_size;
SDL_Texture *block_texture[TEXTURE_NUM];
extern SDL_Renderer *main_renderer;

void set_block_size(unsigned short bs)
{
    block_size = bs;
}

/* Draw block according to the position in map */
void draw_block(BLOCK b, unsigned short y, unsigned short x)
{
    draw_block_helper(main_renderer, block_texture[b], y, x);
}

static void draw_block_helper(SDL_Renderer *r, SDL_Texture *t, unsigned short y, unsigned short x)
{
    SDL_Rect dst_r = {
        x * block_size,
        y * block_size,
        block_size,
        block_size,
    };
    draw(r, t, NULL, &dst_r);
}

/* Conver window pos to map pos */
void window2map(short *y, short *x)
{
    *y = *y / block_size;
    *x = *x / block_size;
}
