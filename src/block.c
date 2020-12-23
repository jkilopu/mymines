/**
 * @file block.c
 * @author jkilopu
 * @brief Provide block image paths and fuctions to draw block.
 * 
 * TODO: Aribitary origin of coordinates.
 */
#include "block.h"
#include "render.h"

const char *block_image_paths[BLOCK_TEXTURE_NUM] = {
    "res/Background.gif",
    "res/one.gif",
    "res/two.gif",
    "res/three.gif",
    "res/four.gif",
    "res/five.gif",
    "res/six.gif",
    "res/seven.gif",
    "res/eight.gif",
    "res/mine.gif",
    "res/explodedMine.gif",
    "res/ask.gif",
    "res/flag.gif",
    "res/hidden.gif",
};
SDL_Texture *block_textures[BLOCK_TEXTURE_NUM];
static unsigned int block_size;
extern SDL_Renderer *main_renderer;

/**
 * @brief Set the block size.
 * 
 * @param bs The block size.
 * 
 * @note The functions must be called before any other functions in the block module.
*/
void set_block_size(unsigned int bs)
{
    block_size = bs;
}

/**
 * @brief Draw block according to the position.
 * 
 * @param b The block enum.
 * @param y The yth block pos on y axis.
 * @param x The xth block pos on x axis.
*/
void draw_block(BLOCK b, unsigned int y, unsigned int x)
{
    SDL_Rect dst_r = {
        x * block_size,
        y * block_size,
        block_size,
        block_size,
    };
    draw(main_renderer, block_textures[b], NULL, &dst_r);
}

/**
 * @brief Convert window pos to map pos.
 * 
 * @param p_y The pointer to window pos on y axis.
 * @param p_x The pointer to window pos on x axis.
 */
void window2map(unsigned int *p_y, unsigned int *p_x)
{
    *p_y = *p_y / block_size;
    *p_x = *p_x / block_size;
}
