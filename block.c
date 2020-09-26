#include "block.h"
#include "render.h"

static Block_settings block_settings;
SDL_Texture *block_texture[TEXTURE_NUM];
extern SDL_Renderer *main_renderer;

void set_block_settings(void)
{
    block_settings.b_size = 30;
    block_settings.o_x = 0;
    block_settings.o_y = 0;
}

/* Draw block according to the position in map */
void draw_block(BLOCK b, unsigned short y, unsigned short x)
{
    draw_block_presizely(main_renderer, block_texture[b], y, x, &block_settings);
}

static void draw_block_presizely(SDL_Renderer *r, SDL_Texture *t, unsigned short y, unsigned short x, Block_settings *bs)
{
    SDL_Rect dst_r = {
        x * bs->b_size + bs->o_x,
        y * bs->b_size + bs->o_y,
        bs->b_size,
        bs->b_size,
    };
    draw(r, t, NULL, &dst_r);
}

/* Conver window pos to map pos */
void window2map(short *y, short *x)
{
    *y = (*y - block_settings.o_y) / block_settings.b_size;
    *x = (*x - block_settings.o_x) / block_settings.b_size;
}
