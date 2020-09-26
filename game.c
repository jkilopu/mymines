#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "game.h"
#include "map.h"
#include "block.h"
#include "render.h"
#include "fatal.h"

#define is_digit(n) (n >= '0' && n <= '9')

extern unsigned short opened_blocks;
extern short directions[8][2];

void setup(void)
{
    init_sdl();
    load_media();

    set_block_settings();
    srand(time(NULL));
}

void start(Map *mP, unsigned short col, unsigned short row, unsigned short n_mines)
{
    *mP = create_map(col, row);
    put_mines(*mP, n_mines);    
    show_unknown((*mP)->col, (*mP)->row);
}

void show_unknown(unsigned short col, unsigned short row)
{ 
   for (int i = 0; i < col; i++)
        for (int j = 0; j < row; j++)
            draw_block(T_HIDDEN, i, j);
}

/* Return true if click on a mine */
bool click_map(Map map, short y, short x, bool *first_click)
{
    if (!in_map_range(y, x, map))
        return false;
    if (*first_click)
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
    if (has_mine(y, x, map))
    {
        draw_block(T_MINE, y, x);
        return true;
    }
    else
        show_block(map, y, x);
    return false;
}

void show_block(Map map, short y, short x)
{
    if (!in_map_range(y, x, map) || map->arr[y][x] == EMPTY || is_digit(map->arr[y][x]))
        return;
    if (map->arr[y][x] >= 1 && map->arr[y][x] <= 8)
    {
        opened_blocks++;
        draw_block(map->arr[y][x], y, x);
        map->arr[y][x] += '0'; // If it is a digit(e.g '2'), it is opened
        return;
    }
    map->arr[y][x] = EMPTY; 
    draw_block(T_BACKGROUND, y, x);
    opened_blocks++;
    for (int i = 0; i < 8; i++)
    {
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
        show_block(map, next_y, next_x);
    }
}

void restart(Map *mP, unsigned short col, unsigned short row, unsigned short n_mines)
{
    destroy_map(mP);
    opened_blocks = 0;
    start(mP, col, row, n_mines);
}

void wrapup(Map *mP)
{
    delete_media();
    finish_sdl();

    destroy_map(mP);
}
