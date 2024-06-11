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

#define add_character(p) ((*(char *)p) == 9 ? (*(char *)p) = 0 : (*(char *)p)++)
#define reduce_character(p) ((*(char *)p) == 0 ? (*(char *)p) = 9 : (*(char *)p)--)

//-------------------------------------------------------------------
// Type Definations
//-------------------------------------------------------------------

typedef void (*SelectFunc)(void *, unsigned short);

typedef struct _option_button {
    SDL_Rect *buttons;
    SDL_Texture **textures;
    void *p_option;
    unsigned short num;
    SelectFunc selector;
} OptionButton;

/**
 * @brief PairButton on the window that is used to change game data.
 */
typedef struct _pair_button
{
    SDL_Rect r;
    unsigned int y_interval; ///< The y interval between buttons.
    void *p_data;           ///< To change the data.
} PairButton;

/**
 * @brief Character on the window that is used to record game data.
 */
typedef struct _character
{
    SDL_Rect r;
    char ch;
} Character;

#endif