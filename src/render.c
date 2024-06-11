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
#include "cursor.h"
#include "menu.h"
#include "fatal.h"

Drawer drawer;

extern const char *block_image_paths[BLOCK_TEXTURE_NUM];
extern SDL_Texture *block_textures[BLOCK_TEXTURE_NUM];
extern SDL_Texture *remote_cursor_texture;
extern SDL_Texture *main_menu_texture;
extern SDL_Texture *local_button_texture;
extern SDL_Texture *lan_button_texture;
extern SDL_Texture *server_button_texture;
extern SDL_Texture *client_button_texture;
extern SDL_Texture *return_button_texture;
extern SDL_Texture *restart_button_texture;
extern SDL_Texture *quit_button_texture;

void drawer_init(int win_width, int win_height, Uint32 win_flag, Uint32 renderer_flag)
{
    drawer.window = SDL_CreateWindow("mymines", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, win_width, win_height, win_flag);
    if (drawer.window == NULL)
        SDL_render_fatal_error("Window could not be created!\n%s\n", SDL_GetError());
    drawer.renderer = SDL_CreateRenderer(drawer.window, -1, renderer_flag);
    if (drawer.renderer == NULL)
        SDL_render_fatal_error("Renderer could not be created!\n%s\n", SDL_GetError());
    SDL_SetRenderDrawColor( drawer.renderer, 0xFF, 0xFF, 0xFF, 0xFF );
}

void drawer_set_window_size(int window_height, int window_width)
{
    SDL_SetWindowSize(drawer.window, window_height, window_width);
}

void drawer_set_logical_size(int logical_width, int logical_height)
{
    if (SDL_RenderSetLogicalSize(drawer.renderer, logical_width, logical_height) < 0)
        SDL_render_fatal_error("Can't set logical size: %s", SDL_GetError());
}

void drawer_add_viewport(SDL_Rect rs[], int num)
{
    if (num > DEFAULT_MAX_VIEWPORT_NUM)
        Error("Viewport must be less than %d, its %d now\n",
        DEFAULT_MAX_VIEWPORT_NUM, num);
    for (int i = 0; i < num; i++)
        drawer.rs[i] = rs[i];
    drawer.viewport_num = num;
}

/**
 * @brief Wrapper for "drawer_load_texture".
 * 
 * @param path The path of the image.
 * 
 * @return The new SDL_Texture.
 */
SDL_Texture *drawer_load_texture(const char *path)
{
    SDL_Texture* new_texture = IMG_LoadTexture(drawer.renderer, path);
    if (new_texture == NULL)
        SDL_render_fatal_error("Unable to load image %s!\n%s\n", path, IMG_GetError());

    return new_texture;
}

/**
 * @brief Wrapper function for "SDL_RenderCopy".
 */
void draw(SDL_Texture *t, const SDL_Rect *src_r, const SDL_Rect *dst_r)
{
    if (SDL_RenderCopy(drawer.renderer, t, src_r, dst_r) != 0)
        SDL_render_fatal_error("Copy error!\n%s\n", SDL_GetError());
}

/**
 * @brief Draw mutiple textures on a renderer.
 * 
 * TODO: Naming?
 */
void draw_mutiple(SDL_Texture *ts[], const SDL_Rect *src_rs, const SDL_Rect *dst_rs, unsigned short num)
{
    if (src_rs == NULL)
        for (unsigned short i = 0; i < num; i++)
            draw(ts[i], NULL, &dst_rs[i]);
    else
        for (unsigned short i = 0; i < num; i++)
            draw(ts[i], &src_rs[i], &dst_rs[i]);
}

void drawer_finit(void)
{
    SDL_DestroyRenderer(drawer.renderer);
    SDL_DestroyWindow(drawer.window);
    drawer.window = NULL;
    drawer.renderer = NULL;
}

/**
 * @brief Init SDL2 with necessary settings.
 */
void init_sdl(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
        SDL_render_fatal_error("SDL could not initialize!\n%s\n", SDL_GetError());
    /* Scale is needed, so try to make it better. */
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        SDL_render_fatal_error("SDL set scale hint error!\n%s\n", SDL_GetError());
    
    drawer_init(MAIN_WIN_SIZE, MAIN_WIN_SIZE, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE, SDL_RENDERER_ACCELERATED);
    drawer_set_logical_size(MAIN_WIN_SIZE, MAIN_WIN_SIZE);
    
    int img_flags = 0; ///< MAYBE: mutiple picture format
    if(!(IMG_Init(img_flags) == img_flags))
        SDL_render_fatal_error("SDL_image could not be initialized\n%s\n", IMG_GetError());
}

/**
 * @brief Load media required throwgh out the game.
 */
void load_media(void)
{
    for (int i = 0; i < BLOCK_TEXTURE_NUM; i++)
        block_textures[i] = drawer_load_texture(block_image_paths[i]);
    remote_cursor_texture = drawer_load_texture(REMOTE_CURSOR_IMG_PATH);
    local_button_texture = drawer_load_texture(LOCAL_BUTTON_PATH);
    main_menu_texture = drawer_load_texture(MAIN_MENU_PATH);
    lan_button_texture = drawer_load_texture(LAN_BUTTON_PATH);
    server_button_texture = drawer_load_texture(SERVER_BUTTON_PATH);
    client_button_texture = drawer_load_texture(CLIENT_BUTTON_PATH);
    return_button_texture = drawer_load_texture(RETURN_BUTTON_PATH);
    restart_button_texture = drawer_load_texture(RESTART_BUTTON_PATH);
    quit_button_texture = drawer_load_texture(QUIT_BUTTON_PATH);
}

/**
 * @brief Set the window size
 * 
 * @param w The window width.
 * @param h The window height.
 *
 * @note Why destroy and reload resoures:
 * The renderer is related to window,
 * and the texture is related to renderer.
 * So I have to delete and reload... That's painful.
 */
//void set_main_window_size(unsigned int w, unsigned int h)
//{
//    SDL_SetWindowSize(drawer.window, (int) w, (int) h);
//
//    SDL_DestroyRenderer(drawer.renderer);
//    delete_media();
//
//    drawer.renderer = SDL_CreateRenderer(drawer.window, -1, SDL_RENDERER_ACCELERATED);
//    if (drawer.renderer == NULL)
//        SDL_render_fatal_error("Renderer could not be created!\n%s\n", SDL_GetError());
//    SDL_SetRenderDrawColor( drawer.renderer, 0xFF, 0xFF, 0xFF, 0xFF );
//    load_media();
//}

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
    SDL_DestroyTexture(main_menu_texture);
    main_menu_texture = NULL;
    SDL_DestroyTexture(local_button_texture);
    local_button_texture = NULL;
    SDL_DestroyTexture(lan_button_texture);
    lan_button_texture = NULL;
    SDL_DestroyTexture(server_button_texture);
    server_button_texture = NULL;
    SDL_DestroyTexture(client_button_texture);
    client_button_texture = NULL;
    SDL_DestroyTexture(return_button_texture);
    return_button_texture = NULL;
    SDL_DestroyTexture(restart_button_texture);
    restart_button_texture = NULL;
    SDL_DestroyTexture(quit_button_texture);
    quit_button_texture = NULL;
}

/**
 * @brief Fishish the SDL.
 */
void finish_sdl(void)
{
    drawer_finit();

    IMG_Quit();
    SDL_Quit();
}
