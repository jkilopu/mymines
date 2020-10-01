#ifndef __BLOCK_H
#define __BLOCK_H

#include "SDL.h"

#define TEXTURE_NUM 14

typedef enum _block
{
    T_BACKGROUND,
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
    T_HIDDEN,
} BLOCK;

void set_block_size(unsigned short bs);
void draw_block(BLOCK b, unsigned short y, unsigned short x);
static void draw_block_helper(SDL_Renderer *r, SDL_Texture *t, unsigned short y, unsigned short x);
void window2map(short *y, short *x);

#endif
