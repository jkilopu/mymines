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
#include "net.h"

#define MAIN_MENU_PATH "res/main_menu.png"
#define LOCAL_BUTTON_PATH "res/local_mode.gif"
#define LAN_BUTTON_PATH "res/lan_mode.gif"
#define SERVER_BUTTON_PATH "res/one.gif"
#define CLIENT_BUTTON_PATH "res/two.gif"
#define RETURN_BUTTON_PATH "res/return.gif"
#define RESTART_BUTTON_PATH "res/restart.gif"
#define QUIT_BUTTON_PATH "res/quit.gif"

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void menu_main(Game game);
static void draw_main_menu(void);

static SDL_bool option(OptionButton *p_opt_btn);
static SDL_bool option_main(SDL_Rect *buttons, void *p_option, unsigned short num, SelectFunc selector);

static SDL_bool play_mode_option(Uint8 *p_game_mode);
static void select_play_mode(void *p_game_mode, unsigned short which);
static SDL_bool connect_mode_option(Uint8 *p_game_mode);
static void select_connect_mode(void *p_game_mode, unsigned short which);

static SDL_bool ip_port_menu(Uint8 game_mode, char ip[], Uint32 *p_port);
static SDL_bool ip_port_menu_main(char ip_port_buf[]);
static SDL_bool parse_ip_port(Uint8 game_mode, const char ip_port_buf[], char *ip, Uint32 *p_port);
static void draw_ip_port(const char *ip_port_buf);

SDL_bool settings_menu(Settings *p_s);
static void draw_settings_menu(const Character ds[], const PairButton bs[], unsigned int num);
static SDL_bool settings_menu_main(Character ds[], PairButton bs[], unsigned int num);

SDL_bool connect_menu(Game game, const char *ip, Uint32 port);
SDL_bool host_menu_main(void);
static void draw_host_menu(unsigned short frame_cnt);

void game_over_menu(void);

#endif