/**
 * @file menu.h
 * @author jkilopu
 * @brief The menu with buttons and options.
 */
#ifndef __MENU_H
#define __MENU_H

#include "SDL.h"
#include "game.h"
#include "button.h"

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void show_menu_and_get_settings(Settings *p_s);
static void draw_menu(Digit ds[], Button bs[], unsigned int num);
static void menu_main(Digit ds[], Button bs[], unsigned int num);

#endif