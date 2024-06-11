/**
 * @file render.h
 * @author jkilopu
 * @brief Core module that can draw something on screen.
 */

#ifndef __RENDER_H
#define __RENDER_H

#include "SDL.h"

#define MAIN_WIN_SIZE 600 ///< Fix intial size
#define DEFAULT_MAX_VIEWPORT_NUM 10

typedef struct _drawer {
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Rect rs[DEFAULT_MAX_VIEWPORT_NUM];
    int viewport_num;
} Drawer;

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void init_sdl(void);
void drawer_init(int win_width, int win_height, Uint32 win_flag, Uint32 renderer_flag);
void drawer_set_logical_size(int logical_width, int logical_height);
void drawer_add_viewport(SDL_Rect rs[], int num);

void load_media(void);
SDL_Texture *drawer_load_texture(const char *path);
void draw(SDL_Texture *t, const SDL_Rect *src_r, const SDL_Rect *dst_r);
void draw_mutiple(SDL_Texture *ts[], const SDL_Rect *src_rs, const SDL_Rect *dst_rs, unsigned short num);
void delete_media(void);

void finish_sdl(void);

#endif
