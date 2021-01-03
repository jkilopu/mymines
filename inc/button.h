/**
 * @file button.h
 * @author jkilopu
 * @brief Header only module. Provied type defination and manipulate macros of "Character" and "PairButton".
 * 
 * @details About PairButton and Character in mymines:
 * A pair of buttons and characters in mymines look like this:
 * ****        (The increase button rect)
 * *  *
 * ****
 *             (This line is the interval)
 * ****         Character rect (to be changed by buttons)
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

#include "SDL_stdinc.h"
#include "SDL.h"

//-------------------------------------------------------------------
// Character Manipulate Macros
//-------------------------------------------------------------------

#define add_character(p_x) ((*p_x) == 9 ? (*p_x) = 0 : (*p_x)++)
#define reduce_character(p_x) ((*p_x) == 0 ? (*p_x) = 9 : (*p_x)--)

//-------------------------------------------------------------------
// Type Definations
//-------------------------------------------------------------------

/**
 * @brief PairButton on the window that is used to change game data, pressed by user.
 */
typedef struct _pair_button
{
    SDL_Rect r;
    unsigned int y_interval; ///< The y interval between buttons.
    char *p_data;           ///< To change the data.
} PairButton;

/**
 * @brief Character on the window that is used to record game data.
 */
typedef struct _character
{
    SDL_Rect r;
    char data; ///< The game data.
} Character;

#endif