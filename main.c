#include <SDL2/SDL.h>
#include <unistd.h>
#include "map.h"
#include "game.h"
#include "block.h"
#include "fatal.h"

static Map map;
unsigned short opened_blocks;
extern SDL_Renderer *main_renderer;

int main(void)
{
    setup();
    SDL_RenderClear(main_renderer);
    start(&map, 16, 16, 40);
    SDL_RenderPresent(main_renderer);

    SDL_Event event;
    bool first_click = true;
    bool quit = false;
    while(!quit)
    {
        if (!SDL_WaitEvent(&event))
            SDL_FatalError("SDL event error!", SDL_GetError());
        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                {
                    int y = 0, x = 0;
                    int state = SDL_GetMouseState(&x, &y);
                    window2map((short *)&y, (short *)&x);
                    switch(state)
                    {
                        case SDL_BUTTON(SDL_BUTTON_LEFT):
                            if(click_map(map, (short) y, (short) x, &first_click))
                            {
                                sleep(5);
                                restart(&map, 16, 16, 40);
                                first_click = true;
                            }
                            if (opened_blocks == map->col * map->row - 40)
                            {
                                sleep(5);
                                restart(&map, 16, 16, 40);
                                first_click = true;
                            }
                        case SDL_BUTTON(SDL_BUTTON_RIGHT):
                            break;
                        default:
                            break;
                    }
                    SDL_RenderPresent(main_renderer);
                }
                break;
            case SDL_MOUSEBUTTONUP: // up button will return state 0
                break;
            case SDL_QUIT:
                quit = true;
                break;
            default:
                break;
        }
    }

    wrapup(&map);
    return 0;
}
