#include "SDL.h"
#include "map.h"
#include "game.h"
#include "block.h"
#include "fatal.h"

static Map map;
extern SDL_Renderer *main_renderer;

int main(int argc, char *argv[])
{
    setup();
    SDL_RenderClear(main_renderer);
    start(&map);
    SDL_RenderPresent(main_renderer);

    SDL_Event event;
    bool first_click = true;
    bool quit = false;
    while(!quit)
    {
        if (!SDL_WaitEvent(&event))
            SDL_FatalError("SDL event error!\n%s\n", SDL_GetError());
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
                            if (click_map(map, (short) y, (short) x, &first_click) || success())
                            {
                                finish(map);
                                SDL_RenderPresent(main_renderer); // show mines
                                SDL_Delay(5000);
                                SDL_RenderClear(main_renderer);
                                restart(&map);
                                first_click = true;
                            }
                            break;
                        case SDL_BUTTON(SDL_BUTTON_RIGHT):
                            set_draw_flag(map, (unsigned short)y, (unsigned short)x);
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
