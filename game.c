#include <ncurses.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "map.h"
#include "game.h"
#include "fatal.h"

#define is_digit(n) (n >= '0' && n <= '9')

extern unsigned short opened_blocks;
extern short directions[8][2];

void setup(void)
{
    initscr();
    crmode();
    cbreak(); // No line buffering
    noecho();
    
    keypad(stdscr, TRUE); // Enable some special events(mouse event here)
    mousemask(BUTTON1_PRESSED | BUTTON3_PRESSED, NULL); // Set the left and right mouse event to be reported 
    if (!has_mouse())
        FatalError("You don't have a mouse!");

    srand(time(NULL));
}

void start(Map *mP, unsigned short col, unsigned short row, unsigned short n_mines)
{
    *mP = create_map(col, row);
    put_mines(*mP, n_mines);    
    show_unknown((*mP)->col, (*mP)->row);

    refresh();
}

void show_unknown(unsigned short col, unsigned short row)
{ 
    for (int i = 0; i < col; i++)
        for (int j = 0; j < row; j++)
            mvaddch(i, j, UNKNOWN);
    move(col, 0);
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
        mvaddch(y, x, MINE);
        return true;
    }
    else
        show_map(map, y, x);
    return false;
}

void show_map(Map map, short y, short x)
{
    if (!in_map_range(y, x, map) || map->arr[y][x] == EMPTY || is_digit(map->arr[y][x]))
        return;
    if (map->arr[y][x] >= 1 && map->arr[y][x] <= 8)
    {
        opened_blocks++;
        map->arr[y][x] += '0'; // If it is a digit(e.g '2'), it is opened
        mvaddch(y, x, map->arr[y][x]);
        return;
    }
    map->arr[y][x] = EMPTY; 
    mvaddch(y, x, EMPTY);
    opened_blocks++;
    for (int i = 0; i < 8; i++)
    {
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
        show_map(map, next_y, next_x);
    }
}

void restart(Map *mP, unsigned short col, unsigned short row, unsigned short n_mines)
{
    destroy_map(mP);
    opened_blocks = 0;
    clear();
    start(mP, col, row, n_mines);
}

void wrapup(Map *mP)
{
    nocrmode();
    nocbreak();
    echo();
    endwin();

    destroy_map(mP);
}
