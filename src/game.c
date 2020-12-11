/**
 * @file game.c
 * @author jkilopu
 * @brief Core logic and settings of game.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>
#include "game.h"
#include "map.h"
#include "block.h"
#include "render.h"
#include "menu.h"
#include "timer.h"
#include "prng_alleged_rc4.h"
#include "fatal.h"

extern const short directions[8][2];

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

/**
 * @brief Create a game with settings.
 * 
 * @param p_settings The settings of the game.
 * 
 * @return The new game.
 * 
 * @note The map in game is not created in this function.
 */
static Game create_game(Settings *p_settings)
{
    Game new_game = malloc_fatal(sizeof(struct _game), "create_game - new_game");

    new_game->settings.block_size = p_settings->block_size;
    new_game->settings.map_height = p_settings->map_height;
    new_game->settings.map_width = p_settings->map_width;
    new_game->settings.n_mine = p_settings->n_mine;
    new_game->settings.window_height = p_settings->window_height;
    new_game->settings.window_width = p_settings->window_width;

    new_game->opened_blocks = 0;

    return new_game;
}

/**
 * @brief Set up all things to start the game.
 * 
 * @return The new game.
 */
Game setup(void)
{
    init_sdl();
    load_media();
    prng_rc4_seed_time();

    Settings tmp_settings;
    show_menu_and_get_settings(&tmp_settings);
    Game game = create_game(&tmp_settings);
    set_block_size(game->settings.block_size);
    set_main_window_size((int) game->settings.window_width, (int) game->settings.window_height);
    set_timer_pos(&game->timer, game->settings.window_width, game->settings.window_height);
    return game;
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
 * @brief Show col*row unknown blocks.
 * 
 * @param map The map to show.
 */
static void show_unknown(Map map)
{ 
   for (int i = 0; i < map->col; i++)
        for (int j = 0; j < map->row; j++)
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
   for (int i = 0; i < map->col; i++)
        for (int j = 0; j < map->row; j++)
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
 * @return Return true if click on a mine.
 */
bool click_map(Game game, short y, short x, bool *first_click)
{
    if (!in_map_range(y, x, game->map) || has_flag(y, x, game->map))
        return false;
    if (first_click != NULL && *first_click)
    {
        *first_click = false;
        if (has_mine(y, x, game->map))
        {
            /* Remove the first clicked mine */
            remove_mine(game->map, y, x);
            /* Reput */
            put_mines(game->map, 1);
            /* Recount(must after reput) */
            set_num(y, x, game->map, cnt_mines(game->map, y, x));
        }
        draw_timer(&game->timer);
        set_timer(&game->timer);
    }
    if (is_shown(y, x, game->map))
        return open_with_flag(game, y, x);
    if (has_mine(y, x, game->map))
    {
        set_exploded_mine(y, x, game->map);
        draw_block(T_EXPLODED_MINE, y, x);
        return true;
    }
    else
        show_blocks(game, y, x);
    return false;
}

/**
 * @brief Show blocks, follow the rules of Mines. 
 * 
 * @param game The game contains the map.
 * @param y   The column of the selected block.
 * @param x   The row of the selected block.
 */
static void show_blocks(Game game, short y, short x)
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
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
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
 * @return Return true if click on a mine.
 * 
 * @note The value of the block must be a digit.
 */
static bool open_with_flag(Game game, unsigned short y, unsigned short x)
{
    bool step_on_mine = false; // The final state
    bool once = false; // For each auto click
    if (cnt_flags(game->map, y, x) != get_mine_num(y, x, game->map))
        return false;
    for (int i = 0; i < 8; i++)
    {
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
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
 * @return Return true if the player succeed.
 */
bool success(Game game)
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

    destroy_game(game);
}
