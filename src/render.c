/**
 * @file fatal.h
 * @author jkilopu
 * @brief Provide functions to start and finish SDL and resources, wrapped functions to draw on window.
 * 
 * @note About SDL2 and mymines:
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

SDL_Renderer *main_renderer;
static SDL_Window *main_window;
extern const char *block_image_paths[BLOCK_TEXTURE_NUM];
extern SDL_Texture *block_textures[BLOCK_TEXTURE_NUM];
extern FILE *output;

/**
 * @brief Init SDL2 with necessary settings.
 */
void init_sdl(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
        SDL_output_fatal_error("SDL could not initialize!\n%s\n", SDL_GetError());
    /* Scale is needed, so try to make it better. */
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        SDL_output_error("SDL set scale hint error!\n%s\n", SDL_GetError());
    
    main_window = SDL_CreateWindow("mymines", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, MAIN_WIN_SIZE, MAIN_WIN_SIZE, SDL_WINDOW_SHOWN);
    if (main_window == NULL)
        SDL_output_fatal_error("Window could not be created!\n%s\n", SDL_GetError());
    main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (main_renderer == NULL)
        SDL_output_fatal_error("Renderer could not be created!\n%s\n", SDL_GetError());
    SDL_SetRenderDrawColor( main_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    
    int img_flags = 0; ///< MAYBE: mutiple picture format
    if(!(IMG_Init(img_flags) == img_flags))
        SDL_output_fatal_error("SDL_image could not be initialized\n%s\n", IMG_GetError());
}

/**
 * @brief Load SDL_Texture from path.
 * 
 * @param path The path of the image.
 * 
 * @return The new SDL_Texture.
 */
SDL_Texture *load_texture(const char *path)
{
    SDL_Texture* new_texture = NULL;
    SDL_Surface* loaded_surface = IMG_Load(path);

    if(loaded_surface == NULL)
        SDL_output_fatal_error("Unable to load image %s!\n%s\n", path, IMG_GetError());
    new_texture = SDL_CreateTextureFromSurface(main_renderer, loaded_surface);
    if(new_texture == NULL)
        SDL_output_fatal_error("Unable to create texture from %s!\n%s\n", path, SDL_GetError());

    SDL_FreeSurface(loaded_surface);
    return new_texture;
}

/**
 * @brief Load media required throwgh out the game.
 */
void load_media(void)
{
    for (int i = 0; i < BLOCK_TEXTURE_NUM; i++)
        block_textures[i] = load_texture(block_image_paths[i]);
}

void draw(SDL_Renderer *r, SDL_Texture *t,  SDL_Rect *src_r, SDL_Rect *dst_r)
{
    if (SDL_RenderCopy(r, t, src_r, dst_r) != 0)
        SDL_output_error("Copy error!\n%s\n", SDL_GetError());
}

/**
 * @brief Set the window size
 * 
 * @param w The window width.
 * @param h The window height.
 *
 * @note Why destroy and reload the resoures:
 * The renderer is related to window,
 * and the texture is related to renderer.
 * So I have to delete and reload... That's painful.
 */
void set_main_window_size(int w, int h)
{
    SDL_SetWindowSize(main_window, w, h);

    SDL_DestroyRenderer(main_renderer);
    delete_media();

    main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
    if (main_renderer == NULL)
        SDL_output_fatal_error("Renderer could not be created!\n%s\n", SDL_GetError());
    SDL_SetRenderDrawColor( main_renderer, 0xFF, 0xFF, 0xFF, 0xFF );
    load_media();
}

/**
 * @brief Delete media that is loaded by "load_media".
 */
void delete_media(void)
{
    for (int i = 0; i < BLOCK_TEXTURE_NUM; i++)
    {
        SDL_DestroyTexture(block_textures[i]);
        block_textures[i] = NULL;
    }
}

/**
 * @brief Fishish the SDL.
 */
void finish_sdl(void)
{
    SDL_DestroyRenderer(main_renderer);
    SDL_DestroyWindow(main_window);
    main_window = NULL;
    main_renderer = NULL;

    IMG_Quit();
    SDL_Quit();
}
