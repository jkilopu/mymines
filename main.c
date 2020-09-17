#include <ncurses.h>
#include <unistd.h>
#include "map.h"
#include "game.h"
#include "fatal.h"

static Map map;
unsigned short opened_blocks;
static unsigned short n_mines;

int main(void)
{
    setup();
    start(&map, 10, 10, 10);

    int ch;
    MEVENT event;
    bool first_click = true;
    while((ch = getch()) != 'q')
    {
        switch(ch)
        {
            case KEY_MOUSE:
                if (getmouse(&event) == OK)
                {
                    if (event.bstate & BUTTON1_PRESSED)
                    {
                        if(click_map(map, (short) event.y, (short) event.x, &first_click))
                        {
                            mvaddnstr(map->col, 0, "Bomb!", 10);
                            refresh();
                            sleep(5);
                            restart(&map, 10, 10, 10);
                            first_click = true;
                        }
                        if (opened_blocks == map->col * map->row - 10)
                        {
                            mvaddnstr(map->col, 0, "You win!", 10);
                            refresh();
                            sleep(5);
                            restart(&map, 10, 10, 10);
                            first_click = true;
                        }
                        /*
                        char buf[10];
                        sprintf(buf, "%u", opened_blocks);
                        mvaddstr(map->col, 0, buf);
                        move(map->col, 0);
                        refresh();
                        */
                        move(map->col, 0);
                        refresh();
                    }
                    else if (event.bstate & BUTTON3_CLICKED)
                    {
                    }
                }
                else
                    Error("getmouse()");
                break;
            case ERR:
                Error("What?");
                break;
            default:
                break;
        }
    }

    wrapup(&map);
    return 0;
}
