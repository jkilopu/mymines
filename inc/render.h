/**
 * @file render.h
 * @author jkilopu
 * @brief Core module that can draw something on screen.
 */

#ifndef __RENDER_H
#define __RENDER_H

#include "SDL.h"

#define MAIN_WIN_SIZE 600 ///< Fix intial size

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void init_sdl(void);

SDL_Texture *load_texture(const char *path);
void load_media(void);
void draw(SDL_Renderer *r, SDL_Texture *t,  SDL_Rect *src_r, SDL_Rect *dst_r);
void set_main_window_size(unsigned int w, unsigned int h);
void delete_media(void);

void finish_sdl(void);

void game_over_menu(void);

#endif
