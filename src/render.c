/*
 * note:
 * 1. SDL is actually quiet easy to use.(Despite a lot of setup and error detection)
 * 2. Still have questions in global variables and module building, is there a most confortable and efficient way?
 * 3. Some source file merely change.(Thanks to modularizition developing?)
 * 4. No serious bug found while developing.
 */

#include "SDL.h"
#include "SDL_image.h"
#include "render.h"
#include "block.h"
#include "fatal.h"

SDL_Window *main_window;
SDL_Renderer *main_renderer;
extern SDL_Texture *block_texture[TEXTURE_NUM];

void init_sdl(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        SDL_FatalError("SDL could not initialize!\n%s", SDL_GetError());
    // Scale is needed, so try to make it better.
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        SDL_Error("SDL set scale hint error!\n%s", SDL_GetError());
    
    main_window = SDL_CreateWindow("mymines", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, MAIN_WIN_WIDTH, MAIN_WIN_HEIGHT, SDL_WINDOW_SHOWN);
    if (main_window == NULL)
        SDL_FatalError("Window could not be created!\n%s", SDL_GetError());
    main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (main_renderer == NULL)
        SDL_FatalError("Renderer could not be created!", SDL_GetError());
    SDL_SetRenderDrawColor( main_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    
    int img_flags = 0; // MAYBE: mutiple picture format
    if(!(IMG_Init(img_flags) == img_flags))
        SDL_FatalError("SDL_image could not be initialized", IMG_GetError());
}

SDL_Texture *load_texture(const char *path)
{
    SDL_Texture* new_texture = NULL;
    SDL_Surface* loaded_surface = IMG_Load(path);

    if(loaded_surface == NULL)
    {
        fprintf(stderr, "Unable to load image %s!\n%s\n", path, IMG_GetError());
        exit(1);
    }
    new_texture = SDL_CreateTextureFromSurface(main_renderer, loaded_surface);
    if(new_texture == NULL)
    {
        fprintf(stderr, "Unable to create texture from %s!\n%s\n", path, SDL_GetError());
        exit(1);
    }

    SDL_FreeSurface(loaded_surface);
    return new_texture;
}

void load_media(void)
{
    char *b_paths[TEXTURE_NUM] = {
        "res/Background.gif",
        "res/one.gif",
        "res/two.gif",
        "res/three.gif",
        "res/four.gif",
        "res/five.gif",
        "res/six.gif",
        "res/seven.gif",
        "res/eight.gif",
        "res/mine.gif",
        "res/explodedMine.gif",
        "res/ask.gif",
        "res/flag.gif",
        "res/hidden.gif",
    };
    for (int i = 0; i < TEXTURE_NUM; i++)
        block_texture[i] = load_texture(b_paths[i]);
}

void draw(SDL_Renderer *r, SDL_Texture *t,  SDL_Rect *src_r, SDL_Rect *dst_r)
{
    if (SDL_RenderCopy(r, t, src_r, dst_r) != 0)
        SDL_Error("Copy error!", SDL_GetError());
}

/* Set the window size
 * The renderer is related to window,
 * and the texture is related to renderer.
 * So I have to delet and reload... That's painful.
 */
void set_main_window_size(int w, int h)
{
    SDL_SetWindowSize(main_window, w, h);

    SDL_DestroyRenderer(main_renderer);
    delete_media();

    main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (main_renderer == NULL)
        SDL_FatalError("Renderer could not be created!", SDL_GetError());
    SDL_SetRenderDrawColor( main_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    load_media();
}

void delete_media(void)
{
    for (int i = 0; i < TEXTURE_NUM; i++)
    {
        SDL_DestroyTexture(block_texture[i]);
        block_texture[i] = NULL;
    }
}

void finish_sdl(void)
{
    SDL_DestroyRenderer(main_renderer);
    SDL_DestroyWindow(main_window);
    main_window = NULL;
    main_renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}
