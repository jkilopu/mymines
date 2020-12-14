/**
 * @file game.c
 * @author jkilopu
 * @brief Core logic and settings of game.
 */

#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "map.h"
#include "block.h"
#include "render.h"
#include "menu.h"
#include "timer.h"
#include "prng_alleged_rc4.h"
#include "net.h"
#include "SDL_stdinc.h"
#include "fatal.h"

extern const short directions[8][2];

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

/**
 * @brief Create a game with game mode.
 * 
 * @param p_settings The mode settings of the game.
 * 
 * @return The new game with empty settings and map.
 */
static Game create_game_with_mode(Uint8 game_mode)
{
    Game new_game = malloc_fatal(sizeof(struct _game), "create_game - new_game");

    new_game->settings.game_mode = game_mode;

    return new_game;
}

/**
 * @brief Set up all things to start the game.
 * 
 * @return The new game.
 */
Game setup(int argc)
{
    init_sdl();
    load_media();

    Uint8 game_mode;
    /** TODO: User choose the game mode */
    if (argc == 1)
        set_local_mode(game_mode);
    else
    {
        set_lan_mode(game_mode);
        /** TODO: Graphic menu to choose client or server */
        if (argc == 2)
            set_client_mode(game_mode);
        else
            set_server_mode(game_mode);
    }

    Game game = create_game_with_mode(game_mode);
    connect_and_complete_setup(game);

    set_block_size(game->settings.block_size);
    set_main_window_size(game->settings.window_width, game->settings.window_height);
    set_timer_pos(&game->timer, game->settings.window_width, game->settings.window_height);
    return game;
}

void connect_and_complete_setup(Game game)
{
    if (!is_lan_mode(game->settings.game_mode))
    {
        prng_rc4_seed_time();
        show_menu_and_get_settings(&game->settings);
        return;
    }

    if (SDLNet_Init() < -1)
        SDL_net_error("SDL_Net could not initialize!\n%s\n", SDL_GetError());

    SeedKeyPacket seed_key_packet;
    if (is_server_mode(game->settings.game_mode))
    {
        seed_key_packet.key = (Uint64) time(NULL);
        seed_key_packet.key_size = (Uint8) sizeof(time_t);
        prng_rc4_seed_bytes(&seed_key_packet.key, seed_key_packet.key_size);

        show_menu_and_get_settings(&game->settings);

        host_game(7777, &seed_key_packet, &game->settings);
    }
    else
    {
        join_game("192.168.1.100", 7777, &seed_key_packet, &game->settings);
        prng_rc4_seed_bytes(&seed_key_packet.key, seed_key_packet.key_size);
    }
}

/**
 * @brief Create the map in game.
 * 
 * @param game The game which is already set up.
 * 
 */
void create_map_in_game(Game game)
{
    game->map = create_map(game->settings.map_height, game->settings.map_width);
    put_mines(game->map, game->settings.n_mine);    
    show_unknown(game->map);
}

/**
 * @brief Receive packet from remote and 
 * 
 * @param game The running game.
 * @param first_click If the click is first click.
 * 
 * @return Return SDL_TRUE if the window need to update.
 */
SDL_bool handle_recved_packet(Game game, SDL_bool *first_click)
{
    PacketType packet_type = recv_packet_type();
    switch (packet_type)
    {
    case TYPE_NONE:
        break;
    case TYPE_MAP_POS:
    {
        unsigned short y, x;
        ClickType click_type = recv_click_map_packet(&y, &x);
        switch (click_type)
        {
        case LEFT_CLICK:
            if (click_map(game, y, x, first_click) || success(game))
            {
                finish(game);
                game_over_menu();
                restart(game);
                *first_click = SDL_TRUE;
            }
            break;
        case RIGHT_CLICK:
            set_draw_flag(game, y, x);
            break;
        default:
            break;
        }
        return SDL_TRUE;
    }
    case TYPE_QUIT:
        /** TODO: Show that other side quits on the screen */
        finish_sdl_net();
        set_local_mode(game->settings.game_mode);
        SDL_Log("The other side has quit.\n");
        return SDL_TRUE;
    default:
        break;
    }
    return SDL_FALSE;
}

/**
 * @brief Show col*row unknown blocks.
 * 
 * @param map The map to show.
 */
static void show_unknown(Map map)
{ 
   for (unsigned short i = 0; i < map->col; i++)
        for (unsigned short j = 0; j < map->row; j++)
            draw_block(T_HIDDEN, i, j);
}

/**
 * @brief Show all the mines except the exploded one.
 * 
 * @param map The col*row map.
 * 
 * @note Since I didn't record the pos of all mines,
 * (because its useless in other conditions)
 * I iterate the map to show all mines.
 * (though it may be inefficient)
 */
static void show_mines(Map map)
{
   for (unsigned short i = 0; i < map->col; i++)
        for (unsigned short j = 0; j < map->row; j++)
        {
            if (has_flag(i, j, map))
                unset_flag(i, j, map);
            if (has_mine(i, j, map))
                draw_block(T_MINE, i, j);
        }
}

/**
 * @brief "Click" a block in the map, and dertermine if it is first click.
 * 
 * @param game The game contains map.
 * @param y   The column number of clicked block.
 * @param x   The row number of clicked mine.
 * @param first_click If the click is first click.
 *  
 * @return Return SDL_TRUE if click on a mine.
 */
SDL_bool click_map(Game game, unsigned short y, unsigned short x, SDL_bool *first_click)
{
    if (!in_map_range(y, x, game->map) || has_flag(y, x, game->map))
        return SDL_FALSE;
    if (first_click != NULL && *first_click)
    {
        *first_click = SDL_FALSE;
        if (has_mine(y, x, game->map))
        {
            /* Remove the first clicked mine */
            remove_mine(game->map, y, x);
            /* Reput */
            put_mines(game->map, 1);
            /* Recount(must after reput) */
            set_num(y, x, game->map, cnt_mines(game->map, y, x));
        }
        set_timer(&game->timer);
        draw_timer(&game->timer);
    }
    if (is_shown(y, x, game->map))
        return open_with_flag(game, y, x);
    if (has_mine(y, x, game->map))
    {
        set_exploded_mine(y, x, game->map);
        draw_block(T_EXPLODED_MINE, y, x);
        return SDL_TRUE;
    }
    else
        show_blocks(game, y, x);
    return SDL_FALSE;
}

/**
 * @brief Show blocks, follow the rules of Mines. 
 * 
 * @param game The game contains the map.
 * @param y   The column of the selected block.
 * @param x   The row of the selected block.
 */
static void show_blocks(Game game, unsigned short y, unsigned short x)
{
    if (!in_map_range(y, x, game->map))
        return;
    if (has_flag(y, x, game->map)) ///< For block REACHED by "show_blocks" (not CLICKED)
        unset_flag(y, x, game->map);
    if (is_shown(y, x, game->map))
        return;
    game->opened_blocks++;
    draw_block(get_block(y, x, game->map), y, x);
    open_block(y, x, game->map); ///< If it is a digit(e.g '2'), it is opened
    if (!is_empty(y, x, game->map))
        return;
    for (int i = 0; i < 8; i++)
    {
        unsigned short next_y = y + directions[i][0];
        unsigned short next_x = x + directions[i][1];
        show_blocks(game, next_y, next_x);
    }
}

/**
 * @brief Set and draw the flag at selected block in the map.
 * 
 * @param game The game contains the map.
 * @param y   The column of the selected block.
 * @param x   The row of the selected block.
 */
void set_draw_flag(Game game, unsigned short y, unsigned short x)
{
    if (!in_map_range(y, x, game->map) || is_shown(y, x, game->map))
        return;
    if (has_flag(y, x, game->map))
    {
        unset_flag(y, x, game->map);
        draw_block(T_HIDDEN, y, x);
    }
    else
    {
        set_flag(y, x, game->map);
        draw_block(T_FLAG, y, x);
    }
}

/**
 * @brief Open blocks (without flag) around the selected block.
 * 
 * @param game The game contains the map.
 * @param y   The column of the selected block.
 * @param x   The row of the selected block.
 * 
 * @return Return SDL_TRUE if click on a mine.
 * 
 * @note The value of the block must be a digit.
 */
static SDL_bool open_with_flag(Game game, unsigned short y, unsigned short x)
{
    SDL_bool step_on_mine = SDL_FALSE; // The final state
    SDL_bool once = SDL_FALSE; // For each auto click
    if (cnt_flags(game->map, y, x) != get_mine_num(y, x, game->map))
        return SDL_FALSE;
    for (int i = 0; i < 8; i++)
    {
        unsigned short next_y = y + directions[i][0];
        unsigned short next_x = x + directions[i][1];
        if(in_map_range(next_y, next_x, game->map) && !is_shown(next_y, next_x, game->map)) // I just want to open surroundings(8 blocks)
            once = click_map(game, next_y, next_x, NULL);
        step_on_mine = (step_on_mine ? step_on_mine : once);
    }
    return step_on_mine;
}

/**
 * @brief See if the player succeed.
 * 
 * @param game The game.
 * 
 * @return Return SDL_TRUE if the player succeed.
 */
SDL_bool success(Game game)
{
    return game->opened_blocks == game->settings.map_width * game->settings.map_height - game->settings.n_mine;
}

/**
 * @brief Destroy a game.
 * 
 * @param game The game to destory.
 */
static void destroy_game(Game game)
{
    destroy_map(game->map);
    game->map = NULL;
    free(game);
}

/**
 * @brief Finish the game process.
 * 
 * @param game The game to finish.
 */
void finish(Game game)
{
    unset_timer(&game->timer);
    show_mines(game->map);
}

/**
 * @brief Restart the game.
 * 
 * @param game The game contains the map to clear.
 */
void restart(Game game)
{
    game->opened_blocks = 0;
    clear_map(game->map);
    put_mines(game->map, game->settings.n_mine);    
    show_unknown(game->map);
}

/**
 * @brief Wrapup the game resource.
 * 
 * @param game The game to destroy.
 */
void wrapup(Game game)
{
    delete_media();
    finish_sdl();
    if (is_lan_mode(game->settings.game_mode))
        finish_sdl_net();

    destroy_game(game);
}
