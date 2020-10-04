#ifndef __MENU_H
#define __MENU_H

#include "SDL.h"
#include "game.h"
#include "button.h"

void get_settings(Settings *p_s);
static void draw_menu(Digit ds[], Button bs[], int num);
static void change_digits(Digit ds[], Button bs[], int num);

#endif