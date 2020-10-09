#include "menu.h"
#include "SDL.h"
#include "render.h"
#include "block.h"
#include "button.h"
#include "timer.h"
#include "fatal.h"
#include <stdbool.h>

static SDL_Texture *menu;
extern SDL_Texture *block_texture[];
extern SDL_Renderer *main_renderer;

void get_settings(Settings *p_s)
{
    Digit digits[] = {
        {{400, 150, 60, 60}, 0},
        {{500, 150, 60, 60}, 0},
        {{400, 300, 60, 60}, 0},
        {{500, 300, 60, 60}, 0},
        {{400, 450, 60, 60}, 0},
        {{500, 450, 60, 60}, 0},
    };
    Button buttons[] = {
        {{415, 130, 30, 30}, 70, &digits[0].data},
        {{515, 130, 30, 30}, 70, &digits[1].data},
        {{415, 280, 30, 30}, 70, &digits[2].data},
        {{515, 280, 30, 30}, 70, &digits[3].data},
        {{415, 430, 30, 30}, 70, &digits[4].data},
        {{515, 430, 30, 30}, 70, &digits[5].data},
    };
    
    draw_menu(digits, buttons, 6);
    change_digits(digits, buttons, 6);
    
    p_s->map_width = digits[0].data * 10 + digits[1].data;
    p_s->map_height = digits[2].data * 10 + digits[3].data;
    p_s->n_mine = digits[4].data * 10 + digits[5].data;
    if (!p_s->map_width || !p_s->map_height || !p_s->n_mine || p_s->n_mine >= p_s->map_width * p_s->map_height)
        Error("Invalid option!");
    p_s->block_size = MAIN_WIN_SIZE / (p_s->map_width > p_s->map_height ? p_s->map_width : p_s->map_height);
    p_s->window_height = p_s->map_height * p_s->block_size; 
    p_s->window_width = p_s->map_width * p_s->block_size + TIME_REGION_WIDTH;
}

static void draw_menu(Digit ds[], Button bs[], int num)
{
    menu = load_texture("res/menu.gif");
    draw(main_renderer, menu, NULL, NULL);
    
    for (int i = 0; i < num; i++)
    {
        draw(main_renderer, block_texture[T_BACKGROUND], NULL, &ds[i].r);
        SDL_Rect tmp_r = bs[i].r;
        tmp_r.y += bs[i].y_interval;
        draw(main_renderer, block_texture[T_FLAG], NULL, &bs[i].r);
        draw(main_renderer, block_texture[T_FLAG], NULL, &tmp_r);
    }
    
    SDL_RenderPresent(main_renderer);
    SDL_DestroyTexture(menu);
    menu = NULL;
}

static void change_digits(Digit ds[], Button bs[], int num)
{
    bool done = false;
    SDL_Event e;
    while (!done)
    {
        if (!SDL_WaitEvent(&e))
            SDL_FatalError("SDL event error!\n%s\n", SDL_GetError());
        if (e.type == SDL_MOUSEBUTTONDOWN)
        {
            int y = 0, x = 0;
            int state = SDL_GetMouseState(&x, &y);
            int selected = -1;
            if (state = SDL_BUTTON(SDL_BUTTON_LEFT))
            {
                SDL_Point point = {x, y};
                for (int i = 0; i < num; i++)
                {
                    SDL_Rect tmp_r = bs[i].r;
                    tmp_r.y += bs[i].y_interval;
                    if (SDL_PointInRect(&point, &bs[i].r))
                    {
                        selected = i;
                        add_digit(bs[i].p_data);
                        break;
                    }
                    else if (SDL_PointInRect(&point, &tmp_r))
                    {
                        selected = i;
                        reduce_digit(bs[i].p_data);
                        break;
                    }
                }
            }
            if (selected != -1)
            {
                draw(main_renderer, block_texture[ds[selected].data], NULL, &ds[selected].r);
                SDL_RenderPresent(main_renderer);
            }
            else
                done = true;
        }
        else if(e.type == SDL_QUIT)
        {
            delete_media();
            finish_sdl();
            exit(0);
        }
    }
}
