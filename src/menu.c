/**
 * @file menu.c
 * @author jkilopu
 * @brief The menu with buttons and options.
 */
#include "menu.h"
#include "SDL.h"
#include "render.h"
#include "block.h"
#include "button.h"
#include "timer.h"
#include "fatal.h"
#include <stdbool.h>

static SDL_Texture *menu; ///< The menu texture.
extern SDL_Texture *block_textures[]; ///< Used as options and digits.
extern SDL_Renderer *main_renderer;
extern FILE *output;

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

/**
 * @brief Core functions which show the menu and get settings.
 * 
 * @param p_s The settings will get from the user.
*/
void show_menu_and_get_settings(Settings *p_s)
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
    menu_main(digits, buttons, 6);
    
    p_s->map_width = digits[0].data * 10 + digits[1].data;
    p_s->map_height = digits[2].data * 10 + digits[3].data;
    p_s->n_mine = digits[4].data * 10 + digits[5].data;
    if (!p_s->map_width || !p_s->map_height || !p_s->n_mine || p_s->n_mine >= p_s->map_width * p_s->map_height)
        Error("Invalid option!");
    p_s->block_size = MAIN_WIN_SIZE / (p_s->map_width > p_s->map_height ? p_s->map_width : p_s->map_height);
    p_s->window_height = p_s->map_height * p_s->block_size; 
    p_s->window_width = p_s->map_width * p_s->block_size + TIME_REGION_WIDTH;
}

/**
 * @brief Draw the menu with digits and buttons.
 * 
 * @param ds The digit blocks.
 * @param bs The button blocks.
 * @param num The number of ds and bs.
 */
static void draw_menu(Digit ds[], Button bs[], int num)
{
    menu = load_texture("res/menu.gif");
    draw(main_renderer, menu, NULL, NULL);
    
    for (int i = 0; i < num; i++)
    {
        draw(main_renderer, block_textures[T_BACKGROUND], NULL, &ds[i].r);
        SDL_Rect tmp_r = bs[i].r;
        tmp_r.y += bs[i].y_interval;
        draw(main_renderer, block_textures[T_FLAG], NULL, &bs[i].r);
        draw(main_renderer, block_textures[T_FLAG], NULL, &tmp_r);
    }
    
    SDL_RenderPresent(main_renderer);
    SDL_DestroyTexture(menu);
    menu = NULL;
}

/**
 * @brief Wait for user input and draw the menu elements when user inputs arrive.
 * 
 * @param ds The digits will be changed.
 * @param bs The buttons will be pressed.
 * @param num The number of ds and bs.
 */
static void menu_main(Digit ds[], Button bs[], int num)
{
    bool done = false;
    SDL_Event e;
    while (!done)
    {
        if (!SDL_WaitEvent(&e))
            SDL_output_fatal_error("SDL event error!\n%s\n", SDL_GetError());
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
                draw(main_renderer, block_textures[ds[selected].data], NULL, &ds[selected].r);
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
