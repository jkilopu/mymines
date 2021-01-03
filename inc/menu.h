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

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

void ip_port_menu(Uint8 game_mode, char ip[], Uint32 *p_port);
static void ip_port_menu_main(char ip_port_buf[]);
static void parse_ip_port(Uint8 game_mode, const char ip_port_buf[], char *ip, Uint32 *p_port);
static void draw_ip_port(const char *ip_port_buf);
void settings_menu(Settings *p_s);
static void draw_settings_menu(Character ds[], PairButton bs[], unsigned int num);
static void settings_menu_main(Character ds[], PairButton bs[], unsigned int num);
void game_over_menu(void);

#endif