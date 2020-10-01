#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "game.h"
#include "map.h"
#include "block.h"
#include "render.h"
#include "menu.h"
#include "fatal.h"

static Settings settings;
static unsigned short opened_blocks;
extern const short directions[8][2];

void setup(void)
{
    init_sdl();
    load_media();
    srand(time(NULL));

    get_settings(&settings);
    set_block_size(settings.block_size);
    set_main_window_size((int) settings.window_width, (int) settings.window_height);
}

void start(Map *p_m) 
{
    *p_m = create_map(settings.map_height, settings.map_width);
    put_mines(*p_m, settings.n_mine);    
    show_unknown((*p_m)->col, (*p_m)->row);
}

void show_unknown(unsigned short col, unsigned short row)
{ 
   for (int i = 0; i < col; i++)
        for (int j = 0; j < row; j++)
            draw_block(T_HIDDEN, i, j);
}

/* Show all the mines except the exploded one.
 * 
 * Since I didn't record the pos of all mines,
 * (because its useless in other conditions)
 * I iterate the map to show all mines.
 * (though it may be inefficient)
 */
void show_mines(Map map)
{
   for (int i = 0; i < map->col; i++)
        for (int j = 0; j < map->row; j++)
        {
            if (has_flag(map->arr[i][j]))
                unset_flag(map->arr[i][j]);
            if (has_mine(i, j, map) && !has_exploded_mine(i, j, map))
                draw_block(T_MINE, i, j);
        }
}

/* Return true if click on a mine */
bool click_map(Map map, short y, short x, bool *first_click)
{
    if (!in_map_range(y, x, map) || has_flag(map->arr[y][x]))
        return false;
    if (first_click != NULL && *first_click)
    {
        *first_click = false;
        if (has_mine(y, x, map))
        {
            /* Remove the first clicked mine */
            remove_mine(map, y, x);
            /* Reput */
            put_mines(map, 1);
            /* Recount(must after reput)*/
            map->arr[y][x] = cnt_mines(map, y, x);
        }
    }
    if (is_shown(y, x, map))
        return open_with_flag(map, y, x);
    if (has_mine(y, x, map))
    {
        map->arr[y][x] = EXPLODED_MINE;
        draw_block(T_EXPLODED_MINE, y, x);
        return true;
    }
    else
        show_block(map, y, x);
    return false;
}

void show_block(Map map, short y, short x)
{
    if (!in_map_range(y, x, map))
        return;
    if (has_flag(map->arr[y][x])) // For block REACHED by "show_block" (not CLICKED)
        unset_flag(map->arr[y][x]);
    if (is_shown(y, x, map))
        return;
    opened_blocks++;
    draw_block(map->arr[y][x], y, x);
    map->arr[y][x] += '0'; // If it is a digit(e.g '2'), it is opened
    if (!is_empty(y, x, map))
        return;
    for (int i = 0; i < 8; i++)
    {
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
        show_block(map, next_y, next_x);
    }
}

void set_draw_flag(Map map, unsigned short y, unsigned short x)
{
    if (!in_map_range(y, x, map) || is_shown(y, x, map))
        return;
    if (has_flag(map->arr[y][x]))
    {
        unset_flag(map->arr[y][x]);
        draw_block(T_HIDDEN, y, x);
    }
    else
    {
        set_flag(map->arr[y][x]);
        draw_block(T_FLAG, y, x);
    }
}

bool open_with_flag(Map map, unsigned short y, unsigned short x)
{
    bool step_on_mine = false; // The final state
    bool once = false; // For each auto click
    if (cnt_flags(map, y, x) != map->arr[y][x] - '0')
        return false;
    for (int i = 0; i < 8; i++)
    {
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
        if(in_map_range(next_y, next_x, map) && !is_shown(next_y, next_x, map)) // I just want to open surroundings(8 blocks)
            once = click_map(map, next_y, next_x, NULL);
        step_on_mine = (step_on_mine ? step_on_mine : once);
    }
    return step_on_mine;
}

bool success(void)
{
    return opened_blocks == settings.map_width * settings.map_height - settings.n_mine;
}

void restart(Map *p_m)
{
    destroy_map(p_m);
    opened_blocks = 0;
    start(p_m);
}

void wrapup(Map *p_m)
{
    delete_media();
    finish_sdl();

    destroy_map(p_m);
}
