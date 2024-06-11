/**
 * @file menu.c
 * @author jkilopu
 * @brief The menu with buttons and options.
 */
#include "menu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_net.h"
#include "SDL_stdinc.h"
#include "render.h"
#include "block.h"
#include "button.h"
#include "timer.h"
#include "prng_alleged_rc4.h"
#include "fatal.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define OPTION_BUTTON_Y (MAIN_WIN_SIZE / 4 * 3)
#define OPTION_BUTTON_HEIGHT (MAIN_WIN_SIZE / 20)
#define OPTION_BUTTON_WIDTH (OPTION_BUTTON_HEIGHT * 2)
#define LOCAL_BUTTON_X (MAIN_WIN_SIZE / 4)
#define LAN_BUTTON_X (MAIN_WIN_SIZE / 2)
#define SERVER_BUTTON_X (MAIN_WIN_SIZE / 4)
#define CLIENT_BUTTON_X (MAIN_WIN_SIZE / 2)

#define SETTINGS_NUM_SIZE (MAIN_WIN_SIZE / 10)
#define TENS_X (MAIN_WIN_SIZE / 6 * 4)
#define ONES_X (MAIN_WIN_SIZE / 6 * 5)
#define WIDTH_Y (MAIN_WIN_SIZE / 4)
#define HEIGHT_Y (MAIN_WIN_SIZE / 2)
#define N_MINE_Y (MAIN_WIN_SIZE / 4 * 3)
#define BUTTON_SIZE (SETTINGS_NUM_SIZE / 2)
#define TENS_BUTTON_X (TENS_X + SETTINGS_NUM_SIZE / 4)
#define ONES_BUTTON_X (ONES_X + SETTINGS_NUM_SIZE / 4)
#define WIDTH_BUTTON_Y (WIDTH_Y - BUTTON_SIZE / 3 * 2)
#define HEIGTH_BUTTON_Y (HEIGHT_Y - BUTTON_SIZE / 3 * 2)
#define N_MINE_BUTTON_Y (N_MINE_Y - BUTTON_SIZE / 3 * 2)
#define BUTTON_INTERVAL (SETTINGS_NUM_SIZE + BUTTON_SIZE / 3)

#define MAX_TEXT_LEN (MAX_IP_LEN + MAX_PORT_LEN + 1)
#define NUM_WIDTH (MAIN_WIN_SIZE / 30)
#define NUM_HEIGHT (MAIN_WIN_SIZE / 20)
#define DOT_WIDTH (MAIN_WIN_SIZE / 60)
#define DOT_HEIGHT (MAIN_WIN_SIZE / 60)

#define HOST_BLOCK_SIZE (MAIN_WIN_SIZE / 30)
#define HOST_BLOCK_INTERVAL (HOST_BLOCK_SIZE / 2)
#define HOST_BLOCK_X (MAIN_WIN_SIZE / 2 - (HOST_BLOCK_SIZE * 3 + HOST_BLOCK_INTERVAL * 2) / 2)
#define HOST_BLOCK_Y (MAIN_WIN_SIZE / 2 - HOST_BLOCK_SIZE / 2)

extern SDL_Texture *block_textures[]; ///< Used as options and characters.
extern Drawer drawer;
/** TODO: Use a single struct to record textures in menu.*/
SDL_Texture *main_menu_texture;
SDL_Texture *local_button_texture;
SDL_Texture *lan_button_texture;
SDL_Texture *server_button_texture;
SDL_Texture *client_button_texture;
SDL_Texture *return_button_texture;
SDL_Texture *restart_button_texture;
SDL_Texture *quit_button_texture;

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

void menu_main(Game game)
{
    char ip[MAX_IP_LEN + 1];
    Uint32 port = 0;

    /** TODO: Assert settings are set at proper position */
    PLAY_MODE:
    if (!play_mode_option(&game->settings.game_mode))
        exit(0);
    if (is_lan_mode(game->settings.game_mode))
    {
        CONNECT_MODE:
        if (!connect_mode_option(&game->settings.game_mode))
            goto PLAY_MODE;
        IP_PORT_MENU:
        if (!ip_port_menu(game->settings.game_mode, ip, &port))
            goto CONNECT_MODE;
        SETTINGS_MENU:
        if (is_server_mode(game->settings.game_mode))
            if (!settings_menu(&game->settings))
                goto IP_PORT_MENU;
        if (!connect_menu(game, ip, port))
            goto SETTINGS_MENU;
    }
    else
    {
        if (!settings_menu(&game->settings))
            goto PLAY_MODE;
    }
}

static void draw_main_menu(void)
{
    draw(main_menu_texture, NULL, NULL);
}

static SDL_bool option(OptionButton *p_opt_btn)
{
    draw_main_menu();
    draw_mutiple(p_opt_btn->textures, NULL, p_opt_btn->buttons, p_opt_btn->num);
    SDL_RenderPresent(drawer.renderer);

    return option_main(p_opt_btn->buttons, p_opt_btn->p_option, p_opt_btn->num, p_opt_btn->selector);
}

static SDL_bool option_main(SDL_Rect *buttons, void *p_option, unsigned short num, SelectFunc selector)
{
    SDL_Event e;
    SDL_bool finished = SDL_FALSE;
    while (!finished)
    {
        if (!SDL_WaitEvent(&e))
            SDL_other_fatal_error("SDL event error!\n%s\n", SDL_GetError());
        switch (e.type)
        {
        case SDL_MOUSEBUTTONUP:
            if (e.button.state == SDL_RELEASED && e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_Point point = {e.button.x, e.button.y};
                for (unsigned short i = 0; i < num; i++)
                {
                    if (SDL_PointInRect(&point, &buttons[i]))
                    {
                        finished = SDL_TRUE;
                        selector(p_option, i);
                        break;
                    }
                }
            }
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE && e.key.repeat == SDL_FALSE)
                goto Not_Finished;
        case SDL_QUIT:
            exit(0);
        
        /** TODO: Assert here. */
        default:
            break;
        }
    }
    Not_Finished: /** TODO: Quite strange here */
    return finished;
}

static SDL_bool play_mode_option(Uint8 *p_game_mode)
{
    SDL_Rect play_mode_option_rects[2] = {
        {LAN_BUTTON_X, OPTION_BUTTON_Y, OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT},
        {LOCAL_BUTTON_X, OPTION_BUTTON_Y, OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT},
    };
    /** TODO: Assert texture not NULL here. */
    SDL_Texture *play_mode_textures[2] = {local_button_texture, lan_button_texture};
    OptionButton play_mode_opt_btn = {
        play_mode_option_rects,
        play_mode_textures,
        p_game_mode,
        2,
        select_play_mode,
    };

    return option(&play_mode_opt_btn);
    /** TODO: Assert p_game_mode set the mode */
}

static void select_play_mode(void *p_game_mode, unsigned short which)
{
    switch (which)
    {
    case 0:
        set_local_mode(*(Uint8 *)p_game_mode);
        break;
    case 1:
        set_lan_mode(*(Uint8 *)p_game_mode);
        break;
    /** TODO: Assert here */
    default:
        break;
    }
}

static SDL_bool connect_mode_option(Uint8 *p_game_mode)
{
    SDL_Rect connect_mode_option_rects[2] = {
        {SERVER_BUTTON_X, OPTION_BUTTON_Y, OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT},
        {CLIENT_BUTTON_X, OPTION_BUTTON_Y, OPTION_BUTTON_WIDTH, OPTION_BUTTON_HEIGHT},
    };
    /**
     * TODO:
     * 1. Change image.
     * 2. Assert here.
     */
    SDL_Texture *connect_mode_textures[2] = {server_button_texture, client_button_texture};
    OptionButton connect_mode_opt_btn = {
        connect_mode_option_rects,
        connect_mode_textures,
        p_game_mode,
        2,
        select_connect_mode,
    };

    return option(&connect_mode_opt_btn);
    /** TODO: Assert p_game_mode set the mode */
}

static void select_connect_mode(void *p_game_mode, unsigned short which)
{
    switch (which)
    {
    case 0:
        set_server_mode(*(Uint8 *)p_game_mode);
        break;
    case 1:
        set_client_mode(*(Uint8 *)p_game_mode);
        break;
    /** TODO: Assert here */
    default:
        break;
    }
}

static SDL_bool ip_port_menu(Uint8 game_mode, char ip[], Uint32 *p_port)
{
    char ip_port_buf[MAX_TEXT_LEN + 1];
    return ip_port_menu_main(ip_port_buf) && parse_ip_port(game_mode, ip_port_buf, ip, p_port);
}

static SDL_bool ip_port_menu_main(char ip_port_buf[])
{
    unsigned short buf_len = 0;
    SDL_bool finished = SDL_FALSE;
    SDL_Event e;

    SDL_RenderClear(drawer.renderer);
    SDL_RenderPresent(drawer.renderer);
    SDL_StartTextInput();
    /** TODO: Add code for quit */
    while (!finished)
    {
        if (!SDL_WaitEvent(&e))
            SDL_other_fatal_error("SDL event error!\n%s\n", SDL_GetError());
        if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_BACKSPACE && e.key.repeat == SDL_FALSE && buf_len > 0)
            {
                ip_port_buf[--buf_len] = '\0';
                SDL_RenderClear(drawer.renderer);
                draw_ip_port(ip_port_buf);
                SDL_RenderPresent(drawer.renderer);
            }
            else if (e.key.keysym.sym == SDLK_RETURN)
                finished = SDL_TRUE;
            else if (e.key.keysym.sym == SDLK_ESCAPE && e.key.repeat == SDL_FALSE)
                goto Not_Finished;
        }
        else if (e.type == SDL_TEXTINPUT && e.text.text[1] == '\0' && buf_len < MAX_TEXT_LEN)
        {
            char ch = e.text.text[0];
            if (isdigit(ch) || ch == '.' || ch == ':')
            {
                ip_port_buf[buf_len] = ch;
                buf_len += 1;
                ip_port_buf[buf_len] = '\0';
                SDL_RenderClear(drawer.renderer);
                draw_ip_port(ip_port_buf);
                SDL_RenderPresent(drawer.renderer);
            }
        }
        else if (e.type == SDL_QUIT)
            exit(0);
    }

    Not_Finished:
    SDL_StopTextInput();
    return finished;
}

static SDL_bool parse_ip_port(Uint8 game_mode, const char ip_port_buf[], char *ip, Uint32 *p_port)
{
    if (is_server_mode(game_mode))
        *p_port = atoi(ip_port_buf);
    else
    {
        char *p_colon = strchr(ip_port_buf, ':');
        if (p_colon == NULL)
            return SDL_FALSE;
        unsigned short ip_len = p_colon - ip_port_buf;
        strncpy(ip, ip_port_buf, ip_len);
        ip[ip_len] = '\0';
        *p_port = atoi(p_colon + 1);
    }
    return SDL_TRUE;
}

static void draw_ip_port(const char *p_ip_port)
{
    unsigned short len = strlen(p_ip_port);
    SDL_Rect r = {MAIN_WIN_SIZE / 2 - len * (NUM_WIDTH / 2), MAIN_WIN_SIZE / 2, NUM_WIDTH, NUM_HEIGHT};
    
    for (unsigned short i = 0; i < len; i++)
    {
        if (isdigit(p_ip_port[i]))
        {
            draw(block_textures[p_ip_port[i] - '0'], NULL, &r);
            r.x += NUM_WIDTH + DOT_WIDTH / 2;
        }
        else if (p_ip_port[i] == '.')
        {
            r.w = DOT_WIDTH, r.h = DOT_HEIGHT, r.y += NUM_HEIGHT - DOT_HEIGHT;
            draw(block_textures[T_FLAG], NULL, &r);
            r.w = NUM_WIDTH, r.h = NUM_HEIGHT, r.y -= NUM_HEIGHT - DOT_HEIGHT;
            r.x += DOT_WIDTH + DOT_WIDTH / 2;
        }
        else if (p_ip_port[i] == ':')
        {
            draw(block_textures[T_EXPLODED_MINE], NULL, &r);
            r.x += NUM_WIDTH + DOT_WIDTH / 2;
        }
    }
}

/**
 * @brief Core functions which show the menu and get settings.
 * 
 * @param p_s The settings will get from the user.
*/
SDL_bool settings_menu(Settings *p_s)
{
    Character characters[] = {
        {{TENS_X, WIDTH_Y, SETTINGS_NUM_SIZE, SETTINGS_NUM_SIZE}, 0},
        {{ONES_X, WIDTH_Y, SETTINGS_NUM_SIZE, SETTINGS_NUM_SIZE}, 0},
        {{TENS_X, HEIGHT_Y, SETTINGS_NUM_SIZE, SETTINGS_NUM_SIZE}, 0},
        {{ONES_X, HEIGHT_Y, SETTINGS_NUM_SIZE, SETTINGS_NUM_SIZE}, 0},
        {{TENS_X, N_MINE_Y, SETTINGS_NUM_SIZE, SETTINGS_NUM_SIZE}, 0},
        {{ONES_X, N_MINE_Y, SETTINGS_NUM_SIZE, SETTINGS_NUM_SIZE}, 0},
    };
    PairButton buttons[] = {
        {{TENS_BUTTON_X, WIDTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[0].ch},
        {{ONES_BUTTON_X, WIDTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[1].ch},
        {{TENS_BUTTON_X, HEIGTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[2].ch},
        {{ONES_BUTTON_X, HEIGTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[3].ch},
        {{TENS_BUTTON_X, N_MINE_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[4].ch},
        {{ONES_BUTTON_X, N_MINE_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[5].ch},
    };
    
    SDL_RenderClear(drawer.renderer);
    draw_settings_menu(characters, buttons, 6);
    SDL_RenderPresent(drawer.renderer);

    SDL_bool finished = settings_menu_main(characters, buttons, 6);

    if (finished)
    {
        p_s->map_width = characters[0].ch * 10 + characters[1].ch;
        p_s->map_height = characters[2].ch * 10 + characters[3].ch;
        p_s->n_mine = characters[4].ch * 10 + characters[5].ch;
        if (!p_s->map_width || !p_s->map_height || !p_s->n_mine || p_s->n_mine >= p_s->map_width * p_s->map_height)
            Error("Invalid option!\n");
        p_s->block_size = MAIN_WIN_SIZE / (p_s->map_width > p_s->map_height ? p_s->map_width : p_s->map_height);
        p_s->window_height = p_s->map_height * p_s->block_size; 
        p_s->window_width = p_s->map_width * p_s->block_size + TIME_REGION_WIDTH;
    }

    return finished;
}

/**
 * @brief Draw the menu with characters and buttons.
 * 
 * @param ds The character blocks.
 * @param bs The pairbutton blocks.
 * @param num The number of ds and bs.
 */
static void draw_settings_menu(const Character ds[], const PairButton bs[], unsigned int num)
{
    SDL_Texture *menu = drawer_load_texture("res/settings_menu.gif");
    draw(menu, NULL, NULL);
    
    for (unsigned int i = 0; i < num; i++)
    {
        draw(block_textures[T_BACKGROUND], NULL, &ds[i].r);
        SDL_Rect tmp_r = bs[i].r;
        tmp_r.y += bs[i].y_interval;
        draw(block_textures[T_FLAG], NULL, &bs[i].r);
        draw(block_textures[T_FLAG], NULL, &tmp_r);
    }
    
    SDL_DestroyTexture(menu);
    menu = NULL;
}

/**
 * @brief Wait for user input and draw the menu elements when user inputs arrive.
 * 
 * @param ds The characters will be changed.
 * @param bs The buttons will be pressed.
 * @param num The number of ds and bs.
 */
static SDL_bool settings_menu_main(Character ds[], PairButton bs[], unsigned int num)
{
    SDL_bool finished = SDL_FALSE;
    SDL_Event e;
    while (!finished)
    {
        if (!SDL_WaitEvent(&e))
            SDL_other_fatal_error("SDL event error!\n%s\n", SDL_GetError());
        if (e.type == SDL_MOUSEBUTTONUP)
        {
            int selected = -1;
            if (e.button.state == SDL_RELEASED && e.button.button == SDL_BUTTON_LEFT)
            {
                SDL_Point point = {e.button.x, e.button.y};
                for (unsigned int i = 0; i < num; i++)
                {
                    SDL_Rect tmp_r = bs[i].r;
                    tmp_r.y += bs[i].y_interval;
                    if (SDL_PointInRect(&point, &bs[i].r))
                    {
                        selected = i;
                        add_character(bs[i].p_data);
                        break;
                    }
                    else if (SDL_PointInRect(&point, &tmp_r))
                    {
                        selected = i;
                        reduce_character(bs[i].p_data);
                        break;
                    }
                }
            }
            if (selected != -1)
            {
                draw(block_textures[ds[selected].ch], NULL, &ds[selected].r);
                SDL_RenderPresent(drawer.renderer);
            }
            else
                finished = SDL_TRUE;
        }
        else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE && e.key.repeat == SDL_FALSE)
            goto Not_Finished;
        else if (e.type == SDL_QUIT)
            exit(0);
    }

    Not_Finished:
    return finished;
}

/**
 * @brief Start local mode or lan mode, as server or client.
 * 
 * @param game The game with game mode setup.
 * @param ip Used only for client mode.
 * @param port The port to listen on or connect to.
 */
SDL_bool connect_menu(Game game, const char *ip, Uint32 port)
{
    if (SDLNet_Init() < -1)
        SDL_net_error("SDL_Net could not initialize!\n%s\n", SDL_GetError());

    SDL_bool finished;
    Uint64 key;
    Uint8 key_size;
    if (is_server_mode(game->settings.game_mode))
    {
        key = time(NULL);
        key_size = sizeof(time_t);
        prng_rc4_seed_bytes(&key, key_size);
        finished = host_game(port, key, key_size, &game->settings);
    }
    else
    {
        finished = join_game(ip, port, &key, &key_size, &game->settings);
        if (finished)
            prng_rc4_seed_bytes(&key, key_size);
    }
    return finished;
}

/** TODO: Comment: thread not safe. */
SDL_bool host_menu_main(void)
{
    static unsigned short frame_cnt = 0;
    SDL_RenderClear(drawer.renderer);
    /** TODO: Fix frame rate */
    draw_host_menu(((frame_cnt++) / 1000) % 4);
    SDL_RenderPresent(drawer.renderer);

    SDL_Event e;
    while (SDL_PollEvent(&e))
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE && e.key.repeat == SDL_FALSE)
            return SDL_FALSE;
    return SDL_TRUE;
}

static void draw_host_menu(unsigned short frame_cnt)
{
    SDL_Rect r = {HOST_BLOCK_X, HOST_BLOCK_Y, HOST_BLOCK_SIZE, HOST_BLOCK_SIZE};
    for (unsigned short i = 0; i < frame_cnt; i++)
    {
        draw(block_textures[T_MINE], NULL, &r);
        r.x += HOST_BLOCK_INTERVAL + HOST_BLOCK_SIZE;
    }
}

/**
 * @brief Show the gameover menu and wait for user input.
 */
void game_over_menu(void)
{
    SDL_RenderPresent(drawer.renderer); ///< show mines
    SDL_Delay(5000);
    SDL_RenderClear(drawer.renderer);
}
