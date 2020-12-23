/**
 * @file block.h
 * @author jkilopu
 * @brief Provide "drawable" blocks (not blocks in Map), wrapped draw method and block resources.
 */
#ifndef __BLOCK_H
#define __BLOCK_H

#include "SDL.h"

#define BLOCK_TEXTURE_NUM 14

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

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void set_block_size(unsigned int bs);
void draw_block(BLOCK b, unsigned int y, unsigned int x);
void window2map(unsigned int *p_y, unsigned int *p_x);

#endif
