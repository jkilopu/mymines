/**
 * @file menu.c
 * @author jkilopu
 * @brief The menu with buttons and options.
 */
#include "menu.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_stdinc.h"
#include "render.h"
#include "block.h"
#include "button.h"
#include "timer.h"
#include "fatal.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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

static SDL_Texture *menu; ///< The menu texture.
extern SDL_Texture *block_textures[]; ///< Used as options and characters.
extern SDL_Renderer *main_renderer;

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

void ip_port_menu(Uint8 game_mode, char ip[], Uint32 *p_port)
{
    char ip_port_buf[MAX_TEXT_LEN + 1];
    ip_port_menu_main(ip_port_buf);    
    parse_ip_port(game_mode, ip_port_buf, ip, p_port);
}

static void ip_port_menu_main(char ip_port_buf[])
{
    unsigned short buf_len = 0;
    SDL_bool finished = SDL_FALSE;
    SDL_Event e;

    SDL_RenderClear(main_renderer);
    SDL_StartTextInput();
    /** TODO: Add code for quit */
    while (!finished)
    {
        if (!SDL_WaitEvent(&e))
            SDL_other_fatal_error("SDL event error!\n%s\n", SDL_GetError());
        if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_BACKSPACE && buf_len > 0)
            {
                ip_port_buf[--buf_len] = '\0';
                SDL_RenderClear(main_renderer);
                draw_ip_port(ip_port_buf);
                SDL_RenderPresent(main_renderer);
            }
            else if (e.key.keysym.sym == SDLK_RETURN)
                finished = SDL_TRUE;
        }
        else if (e.type == SDL_TEXTINPUT && e.text.text[1] == '\0' && buf_len < MAX_TEXT_LEN)
        {
            char ch = e.text.text[0];
            if (isdigit(ch) || ch == '.' || ch == ':')
            {
                ip_port_buf[buf_len] = ch;
                buf_len += 1;
                ip_port_buf[buf_len] = '\0';
                SDL_RenderClear(main_renderer);
                draw_ip_port(ip_port_buf);
                SDL_RenderPresent(main_renderer);
            }
        }
        else if (e.type == SDL_QUIT)
            exit(0);
    }
    SDL_StopTextInput();
}

static void parse_ip_port(Uint8 game_mode, const char ip_port_buf[], char *ip, Uint32 *p_port)
{
    if (is_server_mode(game_mode))
        *p_port = atoi(ip_port_buf);
    else
    {
        char *p_colon = strchr(ip_port_buf, ':');
        if (p_colon == NULL)
            Error("Illegal input format!\n");
        unsigned short ip_len = p_colon - ip_port_buf;
        strncpy(ip, ip_port_buf, ip_len);
        ip[ip_len] = '\0';
        *p_port = atoi(p_colon + 1);
    }
}

static void draw_ip_port(const char *p_ip_port)
{
    unsigned short len = strlen(p_ip_port);
    SDL_Rect r = {MAIN_WIN_SIZE / 2 - len * (NUM_WIDTH / 2), MAIN_WIN_SIZE / 2, NUM_WIDTH, NUM_HEIGHT};
    
    for (unsigned short i = 0; i < len; i++)
    {
        if (isdigit(p_ip_port[i]))
        {
            draw(main_renderer, block_textures[p_ip_port[i] - '0'], NULL, &r);
            r.x += NUM_WIDTH + DOT_WIDTH / 2;
        }
        else if (p_ip_port[i] == '.')
        {
            r.w = DOT_WIDTH, r.h = DOT_HEIGHT, r.y += NUM_HEIGHT - DOT_HEIGHT;
            draw(main_renderer, block_textures[T_FLAG], NULL, &r);
            r.w = NUM_WIDTH, r.h = NUM_HEIGHT, r.y -= NUM_HEIGHT - DOT_HEIGHT;
            r.x += DOT_WIDTH + DOT_WIDTH / 2;
        }
        else if (p_ip_port[i] == ':')
        {
            draw(main_renderer, block_textures[T_EXPLODED_MINE], NULL, &r);
            r.x += NUM_WIDTH + DOT_WIDTH / 2;
        }
    }
}

/**
 * @brief Core functions which show the menu and get settings.
 * 
 * @param p_s The settings will get from the user.
*/
void settings_menu(Settings *p_s)
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
        {{TENS_BUTTON_X, WIDTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[0].data},
        {{ONES_BUTTON_X, WIDTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[1].data},
        {{TENS_BUTTON_X, HEIGTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[2].data},
        {{ONES_BUTTON_X, HEIGTH_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[3].data},
        {{TENS_BUTTON_X, N_MINE_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[4].data},
        {{ONES_BUTTON_X, N_MINE_BUTTON_Y, BUTTON_SIZE, BUTTON_SIZE}, BUTTON_INTERVAL, &characters[5].data},
    };
    
    draw_settings_menu(characters, buttons, 6);
    settings_menu_main(characters, buttons, 6);
    
    p_s->map_width = characters[0].data * 10 + characters[1].data;
    p_s->map_height = characters[2].data * 10 + characters[3].data;
    p_s->n_mine = characters[4].data * 10 + characters[5].data;
    if (!p_s->map_width || !p_s->map_height || !p_s->n_mine || p_s->n_mine >= p_s->map_width * p_s->map_height)
        Error("Invalid option!\n");
    p_s->block_size = MAIN_WIN_SIZE / (p_s->map_width > p_s->map_height ? p_s->map_width : p_s->map_height);
    p_s->window_height = p_s->map_height * p_s->block_size; 
    p_s->window_width = p_s->map_width * p_s->block_size + TIME_REGION_WIDTH;
}

/**
 * @brief Draw the menu with characters and buttons.
 * 
 * @param ds The digit blocks.
 * @param bs The button blocks.
 * @param num The number of ds and bs.
 */
static void draw_settings_menu(Character ds[], PairButton bs[], unsigned int num)
{
    menu = IMG_LoadTexture(main_renderer, "res/menu.gif");
    draw(main_renderer, menu, NULL, NULL);
    
    for (unsigned int i = 0; i < num; i++)
    {
        draw(main_renderer, block_textures[T_BACKGROUND], NULL, &ds[i].r);
        SDL_Rect tmp_r = bs[i].r;
        tmp_r.y += bs[i].y_interval;
        draw(main_renderer, block_textures[T_FLAG], NULL, &bs[i].r);
        draw(main_renderer, block_textures[T_FLAG], NULL, &tmp_r);
    }
    
    SDL_RenderPresent(main_renderer);
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
static void settings_menu_main(Character ds[], PairButton bs[], unsigned int num)
{
    SDL_bool done = SDL_FALSE;
    SDL_Event e;
    while (!done)
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
                draw(main_renderer, block_textures[ds[selected].data], NULL, &ds[selected].r);
                SDL_RenderPresent(main_renderer);
            }
            else
                done = SDL_TRUE;
        }
        else if (e.type == SDL_QUIT)
            exit(0);
    }
}

/**
 * @brief Show the gameover menu and wait for user input.
 */
void game_over_menu(void)
{
    SDL_RenderPresent(main_renderer); ///< show mines
    SDL_Delay(5000);
    SDL_RenderClear(main_renderer);
}
