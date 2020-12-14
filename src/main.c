/**
 * @file game.h
 * @author jkilopu
 * @brief The main routine of mymines.
 */
#include "SDL.h"
#include "SDL_stdinc.h"
#include "map.h"
#include "game.h"
#include "net.h"
#include "block.h"
#include "render.h"
#include "fatal.h"

static Game game;
extern SDL_Renderer *main_renderer;

int main(int argc, char *argv[])
{
    game = setup(argc);

    SDL_RenderClear(main_renderer);
    create_map_in_game(game);
    SDL_RenderPresent(main_renderer);

    SDL_Event event;
    SDL_bool first_click = SDL_TRUE;
    SDL_bool quit = SDL_FALSE;

    while(!quit)
    {
        while(SDL_PollEvent(&event))
        {
            int y, x;
            int state;
            switch(event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                    state = SDL_GetMouseState(&x, &y);
                    window2map((unsigned short *)&y, (unsigned short *)&x); ///< Not so dangerous pointer cast.
                    break;
                case SDL_MOUSEBUTTONUP: ///< Button up will return state 0.
                    switch(state)
                    {
                        case SDL_BUTTON(SDL_BUTTON_LEFT):
                            if (is_lan_mode(game->settings.game_mode))
                                send_click_map_packet(LEFT_CLICK, y, x);
                            if (click_map(game, y, x, &first_click) || success(game))
                            {
                                finish(game);
                                game_over_menu();
                                restart(game);
                                first_click = SDL_TRUE;
                            }
                            break;
                        case SDL_BUTTON(SDL_BUTTON_RIGHT):
                            if (is_lan_mode(game->settings.game_mode))
                                send_click_map_packet(RIGHT_CLICK, y, x);
                            set_draw_flag(game, y, x);
                            break;
                        default:
                            break;
                    }
                    SDL_RenderPresent(main_renderer);
                    break;
                case SDL_QUIT:
                    if (is_lan_mode(game->settings.game_mode))
                        send_packet_type(TYPE_QUIT);
                    quit = SDL_TRUE;
                    break;
                default:
                    break;
            }
        }
        if (is_lan_mode(game->settings.game_mode))
            if(handle_recved_packet(game, &first_click))
                SDL_RenderPresent(main_renderer);
    }

    wrapup(game);
    return 0;
}
