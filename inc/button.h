#ifndef __BUTTON_H
#define __BUTTON_H

#include <stdint.h>
#include "SDL.h"

#define add_digit(p_x) ((*p_x) == 9 ? (*p_x) = 0 : (*p_x)++)
#define reduce_digit(p_x) ((*p_x) == 0 ? (*p_x) = 9 : (*p_x)--)

/*
 * The button here is like this:
 * ****        (The up button rect)
 * *  *
 * ****
 *             (This line is the interval)
 * digit_rect  (to be changed)
 * 
 * ****
 * *  *
 * ****        (The down button rect)
 * 
 */
typedef struct _button
{
    SDL_Rect r;
    unsigned short y_interval;
    uint8_t *p_data; // I only want to control a small part of data.
} Button;

typedef struct _digit
{
    SDL_Rect r;
    uint8_t data;
} Digit;

#endif