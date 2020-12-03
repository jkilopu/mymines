/**
 * @file button.h
 * @author jkilopu
 * @brief Header only module. Provied type defination and manipulate macros of "Digit" and "Button".
 * 
 * @details About Button and Digit in mymines:
 * A pair of buttons and digits in mymines look like this:
 * ****        (The increase button rect)
 * *  *
 * ****
 *             (This line is the interval)
 * ****         Digit rect (to be changed by buttons)
 * * 1*
 * ****
 *             (This line is the interval)
 * ****
 * *  *
 * ****        (The down button rect)
 * 
 */

#ifndef __BUTTON_H
#define __BUTTON_H

#include <stdint.h>
#include "SDL.h"

//-------------------------------------------------------------------
// Digit Manipulate Macros
//-------------------------------------------------------------------

#define add_digit(p_x) ((*p_x) == 9 ? (*p_x) = 0 : (*p_x)++)
#define reduce_digit(p_x) ((*p_x) == 0 ? (*p_x) = 9 : (*p_x)--)

//-------------------------------------------------------------------
// Type Definations
//-------------------------------------------------------------------

/**
 * @brief Button on the window that is used to change game data, pressed by user.
 */
typedef struct _button
{
    SDL_Rect r;
    unsigned short y_interval; ///< The y interval between buttons.
    uint8_t *p_data; ///< To change the data.
} Button;

/**
 * @brief Digit on the window that is used to record game data.
 */
typedef struct _digit
{
    SDL_Rect r;
    uint8_t data; ///< The game data.
} Digit;

#endif