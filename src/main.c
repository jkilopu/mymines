/**
 * @file game.h
 * @author jkilopu
 * @brief The main routine of mymines.
 */
#include "SDL.h"
#include "map.h"
#include "game.h"
#include "block.h"
#include "fatal.h"

static Game game;
extern SDL_Renderer *main_renderer;
extern FILE *output;

int main(int argc, char *argv[])
{
    if (argc == 2 && strcmp("--error-log", argv[1]) == 0)
        set_fatal_output_stream(true);
    else
        set_fatal_output_stream(false);

    game = setup();
    SDL_RenderClear(main_renderer);
    create_map_in_game(game);
    SDL_RenderPresent(main_renderer);

    SDL_Event event;
    bool first_click = true;
    bool quit = false;
    while(!quit)
    {
        if (!SDL_WaitEvent(&event))
            SDL_output_fatal_error("SDL event error!\n%s\n", SDL_GetError());
        switch(event.type)
        {
            case SDL_MOUSEBUTTONDOWN:
                {
                    int y = 0, x = 0;
                    int state = SDL_GetMouseState(&x, &y);
                    window2map((unsigned short *)&y, (unsigned short *)&x);
                    switch(state)
                    {
                        case SDL_BUTTON(SDL_BUTTON_LEFT):
                            if (click_map(game, (short) y, (short) x, &first_click) || success(game))
                            {
                                finish(game);
                                SDL_RenderPresent(main_renderer); ///< show mines
                                SDL_Delay(5000);
                                SDL_RenderClear(main_renderer);
                                restart(game);
                                first_click = true;
                            }
                            break;
                        case SDL_BUTTON(SDL_BUTTON_RIGHT):
                            set_draw_flag(game, (unsigned short)y, (unsigned short)x);
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

    wrapup(game);
    close_fatal_output_stream();
    return 0;
}
