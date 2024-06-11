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
#include "cursor.h"
#include "menu.h"
#include "render.h"
#include "timer.h"
#include "net.h"
#include "SDL_stdinc.h"
#include "fatal.h"

extern const int directions[8][2];

#define get_block_type_without_mine(y, x, map) (has_flag(y, x, map) ? T_FLAG : \
                                   is_shown_num(y, x, map) ? get_mine_num(y, x, map) : \
                                   is_exploded_mine(y, x, map) ? T_EXPLODED_MINE : T_HIDDEN)

#define get_block_type_all(y, x, map) (has_flag(y, x, map) ? T_FLAG : \
                                   is_shown_num(y, x, map) ? get_mine_num(y, x, map) : \
                                   has_mine(y, x, map) ? T_MINE : \
                                   is_exploded_mine(y, x, map) ? T_EXPLODED_MINE : T_HIDDEN)

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

/**
 * @brief Create a empty game.
 * 
 * @return The new empty game.
 */
static Game create_empty_game(void)
{
    Game new_game = calloc_fatal(1 ,sizeof(struct _game), "create_empty_game - new_game");

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

    Game game = create_empty_game();

    menu_main(game);

    set_block_size(game->settings.block_size);
    set_timer_pos(&game->timer, game->settings.window_width, game->settings.window_height);
    return game;
}

/**
 * @brief Create the map in game.
 * 
 * @param game The game which is already set up.
 */
void create_map_in_game(Game game)
{
    game->map = create_map(game->settings.map_height, game->settings.map_width);
    game->opened_blocks = 0;
    game->is_first_click = SDL_TRUE;
    show_whole_map(game->map);
    put_mines(game->map, game->settings.n_mine);    
}

/**
 * @brief Receive packet from remote and handle it normally according to its type.
 * 
 * @param game The running game.
 * 
 * @return Return SDL_TRUE if the window need to update.
 */
SDL_bool handle_recved_packet(Game game)
{
    static unsigned int last_move_y, last_move_x;
    if (!is_connected_socket_ready())
        return SDL_FALSE;

    MyMinesPacket mymines_packet;
    recv_mymines_packet(&mymines_packet);
    switch (mymines_packet.type)
    {
    case TYPE_NONE:
        Error("TYPE_NONE should not be here!\n");
    case TYPE_CLICK_MAP:
    {
        unsigned int y = mymines_packet.click_map_packet.pos_y;
        unsigned int x = mymines_packet.click_map_packet.pos_x;
        switch (mymines_packet.click_map_packet.click_type)
        {
            case LEFT_CLICK:
                if (click_map(game, y, x) || success(game))
                {
                    finish(game);
                    restart(game);
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
    case TYPE_MOUSE_MOVE:
    {
        show_block_in_cursor(game->map, last_move_y, last_move_x);
        last_move_y = mymines_packet.mouse_move_packet.pos_y;
        last_move_x = mymines_packet.mouse_move_packet.pos_x;
        draw_remote_cursor(last_move_y, last_move_x);
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

static void show_block_in_map_without_mine(Map map, unsigned int y, unsigned int x)
{
    BLOCK b = get_block_type_without_mine(y, x, map);
    draw_block(b, y, x);
}

static void show_block_in_map_all(Map map, unsigned int y, unsigned int x)
{
    BLOCK b = get_block_type_all(y, x, map);
    draw_block(b, y, x);
}

static void show_whole_map(Map map)
{
    for (unsigned int y = 0; y < map->col; y++)
        for (unsigned int x = 0; x < map->row; x++)
            show_block_in_map_all(map, y, x);
}

/**
 * @brief Show the block which is previously occupied by cursor.
 * 
 * @param map The map.
 * @param cursor_y The y coordinate in window.
 * @param cursor_x The x coordinate in window.
 */
static void show_block_in_cursor(Map map, unsigned int cursor_y, unsigned int cursor_x)
{
    unsigned int down_y = cursor_y + CURSOR_HEIGHT, right_x = cursor_x + CURSOR_WIDTH;
    window2map(&cursor_y, &cursor_x);
    window2map(&down_y, &right_x);

    for (unsigned int i = cursor_y; i <= down_y; i++)
        for (unsigned int j = cursor_x; j <= right_x; j++)
            if (in_map_range(i, j, map))
                show_block_in_map_without_mine(map, i, j);
}

/**
 * @brief "Click" a block in the map, and dertermine if it is first click.
 * 
 * @param game The game contains map.
 * @param y   The column number of clicked block.
 * @param x   The row number of clicked mine.
 *  
 * @return Return SDL_TRUE if click on a mine.
 */
SDL_bool click_map(Game game, unsigned int y, unsigned int x)
{
    if (!in_map_range(y, x, game->map) || has_flag(y, x, game->map))
        return SDL_FALSE;
    if (game->is_first_click)
    {
        game->is_first_click = SDL_FALSE;
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
    if (is_shown_num(y, x, game->map))
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
static void show_blocks(Game game, unsigned int y, unsigned int x)
{
    if (!in_map_range(y, x, game->map))
        return;
    if (has_flag(y, x, game->map)) ///< For block REACHED by "show_blocks" (not CLICKED)
        unset_flag(y, x, game->map);
    if (is_shown_num(y, x, game->map))
        return;
    game->opened_blocks++;
    draw_block(get_block(y, x, game->map), y, x);
    open_block(y, x, game->map); ///< If it is a digit(e.g '2'), it is opened
    if (!is_empty(y, x, game->map))
        return;
    for (int i = 0; i < 8; i++)
    {
        unsigned int next_y = y + directions[i][0];
        unsigned int next_x = x + directions[i][1];
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
void set_draw_flag(Game game, unsigned int y, unsigned int x)
{
    if (!in_map_range(y, x, game->map) || is_shown_num(y, x, game->map))
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
static SDL_bool open_with_flag(Game game, unsigned int y, unsigned int x)
{
    SDL_bool step_on_mine = SDL_FALSE; // The final state
    SDL_bool once = SDL_FALSE; // For each auto click
    if (cnt_flags(game->map, y, x) != get_mine_num(y, x, game->map))
        return SDL_FALSE;
    for (int i = 0; i < 8; i++)
    {
        unsigned int next_y = y + directions[i][0];
        unsigned int next_x = x + directions[i][1];
        if(in_map_range(next_y, next_x, game->map) && !is_shown_num(next_y, next_x, game->map)) // I just want to open surroundings(8 blocks)
            once = click_map(game, next_y, next_x);
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
    unhidden_map(game->map);
    show_whole_map(game->map);
    game_over_menu();
}

/**
 * @brief Restart the game.
 * 
 * @param game The game contains the map to clear.
 */
void restart(Game game)
{
    game->opened_blocks = 0;
    game->is_first_click = SDL_TRUE;
    clear_map(game->map);
    show_whole_map(game->map);
    put_mines(game->map, game->settings.n_mine);
    SDL_PumpEvents(); ///< Must call this function before flushing events.
    SDL_FlushEvents(SDL_FIRSTEVENT, SDL_LASTEVENT);
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
