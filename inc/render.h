#ifndef __RENDER_H
#define __RENDER_H

#define MAIN_WIN_WIDTH 800 // LATER: Dynamic size
#define MAIN_WIN_HEIGHT 800

void init_sdl(void);
SDL_Texture *load_texture(const char *path);
void load_media(void);
void draw(SDL_Renderer *r, SDL_Texture *t,  SDL_Rect *src_r, SDL_Rect *dst_r);
void delete_media(void);
void finish_sdl(void);

#endif
