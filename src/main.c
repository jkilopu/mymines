/**
 * @file game.h
 * @author jkilopu
 * @brief The main routine of mymines.
 */
#include "SDL.h"
#include "SDL_stdinc.h"
#include "game.h"
#include "map.h"
#include "render.h"
#include "block.h"
#include "net.h"
#include "fatal.h"

extern SDL_Renderer *main_renderer;

int main(int argc, char *argv[])
{
    Game game = setup(argc, argv);

    SDL_RenderClear(main_renderer);
    create_map_in_game(game);
    SDL_RenderPresent(main_renderer);

    SDL_Event event;
    SDL_bool quit = SDL_FALSE;

    while(!quit)
    {
        if (SDL_PollEvent(&event))
        {
            unsigned int y, x;
            switch(event.type)
            {
                case SDL_MOUSEBUTTONUP: ///< PairButton up will return state 0.
                    y = event.button.y;
                    x = event.button.x;
                    if (event.button.clicks == 1 && event.button.state == SDL_RELEASED)
                    {
                        window2map(&y, &x); ///< Not so dangerous pointer cast.
                        switch(event.button.button)
                        {
                            case SDL_BUTTON_LEFT:
                                if (is_lan_mode(game->settings.game_mode))
                                    send_click_map_packet(LEFT_CLICK, y, x);
                                if (click_map(game, y, x) || success(game))
                                {
                                    finish(game);
                                    restart(game);
                                }
                                break;
                            case SDL_BUTTON_RIGHT:
                                if (is_lan_mode(game->settings.game_mode))
                                    send_click_map_packet(RIGHT_CLICK, y, x);
                                set_draw_flag(game, y, x);
                                break;
                            default:
                                break;
                        }
                        SDL_RenderPresent(main_renderer);
                    }
                    break;
                case SDL_MOUSEMOTION:
                    y = event.motion.y;
                    x = event.motion.x;
                    if (is_lan_mode(game->settings.game_mode))
                        send_mouse_move_packet(y, x);
                    break;
                case SDL_USEREVENT:
                {
                    unsigned int *p_time_passed = event.user.data1;
                    (*p_time_passed)++;
                    draw_timer(&game->timer);
                    SDL_RenderPresent(main_renderer);
                    break;
                }
                case SDL_QUIT:
                    if (is_lan_mode(game->settings.game_mode))
                        send_quit_packet();
                    quit = SDL_TRUE;
                    break;
                default:
                    break;
            }
        }
        if (is_lan_mode(game->settings.game_mode))
            if (handle_recved_packet(game))
                SDL_RenderPresent(main_renderer);
    }

    wrapup(game);
    return 0;
}
