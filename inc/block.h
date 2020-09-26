#ifndef __BLOCK_H
#define __BLOCK_H

#include <SDL2/SDL.h>

#define TEXTURE_NUM 14

typedef enum _block
{
    T_HIDDEN,
    T_ONE,
    T_TWO,
    T_THREE,
    T_FOUR,
    T_FIVE,
    T_SIX,
    T_SEVEN,
    T_EIGHT,
    T_MINE,
    T_EXPLODED_MINE,
    T_ASK,
    T_FLAG,
    T_BACKGROUND,
} BLOCK;

typedef struct _block_settings {
    unsigned short b_size;
    unsigned short o_y;
    unsigned short o_x;
} Block_settings;

void draw_block(BLOCK b, unsigned short y, unsigned short x);
static void draw_block_presizely(SDL_Renderer *r, SDL_Texture *t, unsigned short y, unsigned short x, Block_settings *bs);
void set_block_settings(void);
void window2map(short *y, short *x);

#endif
