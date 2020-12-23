/**
 * @file map.c
 * @author jkilopu
 * @brief Provides functions for the creation and manipulation of the 2d map and its blocks.
 */

#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "prng_alleged_rc4.h"
#include "fatal.h"

#define FAILED_PLACEMENT_MAX_TIMES (10000)

const int directions[8][2] = {{-1, -1}, {-1, 0}, {0, -1}, {1, 1}, {1, 0}, {0, 1}, {1, -1}, {-1, 1}};

//-------------------------------------------------------------------
// Functions
//-------------------------------------------------------------------

/**
 * @brief Create a empty square map with specified size.
 * 
 * @param col The column of the map.
 * @param row The row of the map.
 * 
 * @return The created map.
 * 
 * @warning The param col and row can't be zero.
 */
Map create_map(unsigned int col, unsigned int row)
{
    Map new_map = NULL;
    
    new_map = malloc_fatal(sizeof(struct _map), "create_map - new_map");
    new_map->arr = malloc_fatal(col * sizeof(char *), "create_map - new_map->arr");
    for (unsigned int i = 0; i < col; i++)
        new_map->arr[i] = calloc_fatal(row, sizeof(char), "create_map - new_map->arr[i]");
    
    new_map->col = col;
    new_map->row = row;
    return new_map;
}

/**
 * @brief Put a certain number of mines on a map.
 * 
 * @param map The map to place mines.
 * @param num The number of mines to put.
 * 
 * @warning If tried to place mines too many times, the program will exit.
 */
void put_mines(Map map, unsigned int num)
{
    unsigned int y, x;
    int times = 0;
    
    while(num--)
    {
        do
        {
            y = prng_rc4_get_uint() % map->col;
            x = prng_rc4_get_uint() % map->row;
            times++;
            if (times > FAILED_PLACEMENT_MAX_TIMES)
                Error("Tried too many times");
        } while(has_mine(y, x, map));
        
        set_mine(y, x, map);
        for (int i = 0; i < 8; i++)
        {
            unsigned int next_y = y + directions[i][0];
            unsigned int next_x = x + directions[i][1];
            if(in_map_range(next_y, next_x, map) && !has_mine(next_y, next_x, map))
                map->arr[next_y][next_x]++;
        }
    }
}

/**
 * @brief Remove the (y, x) mines in the map.
 * 
 * @param map The map for which has the specified mine.
 * @param y   The column number of the mine.
 * @param x   The row number of the mine.
 * 
 * @note If there is no mine at (y, x), the program will exit.
 */
void remove_mine(Map map, unsigned int y, unsigned int x)
{
    if (!has_mine(y, x, map))
    {
        fprintf(stderr, "No mine in (%hd, %hd)", y, x);
        exit(1);
    }
    for (int i = 0; i < 8; i++)
    {
        unsigned int next_y = y + directions[i][0];
        unsigned int next_x = x + directions[i][1];
        if(in_map_range(next_y, next_x, map) && !has_mine(next_y, next_x, map))
            map->arr[next_y][next_x]--;
    }
}

/**
 * @brief Count the number of mines arount (i, j).
 * 
 * @param map The map for which has the specified position.
 * @param y   The column number of the position.
 * @param x   The row number of the position.
 */
unsigned int cnt_mines(Map map, unsigned int y, unsigned int x)
{
    unsigned int cnt = 0;
    for (int i = 0; i < 8; i++)
    {
            unsigned int next_y = y + directions[i][0];
            unsigned int next_x = x + directions[i][1];
            if(in_map_range(next_y, next_x, map) && has_mine(next_y, next_x, map))
                cnt++;
    }
    return cnt;
}

/**
 * @brief Count the number of flags arount (i, j).
 * 
 * @param map The map for which has the specified position.
 * @param y   The column number of the position.
 * @param x   The row number of the position.
 */
unsigned int cnt_flags(Map map, unsigned int y, unsigned int x)
{
    unsigned int cnt = 0;
    for (int i = 0; i < 8; i++)
    {
            unsigned int next_y = y + directions[i][0];
            unsigned int next_x = x + directions[i][1];
            if(in_map_range(next_y, next_x, map) && has_flag(next_y, next_x, map))
                cnt++;
    }
    return cnt;
}

void unhidden_map(Map map)
{
    for (unsigned int y = 0; y < map->col; y++)
        for (unsigned int x = 0; x < map->row; x++)
        {
            if (has_flag(y, x, map))
                unset_flag(y, x, map);
            if (is_num(y, x, map))
                open_block(y, x, map);
        }
}

/**
 * @brief Clear the map.
 * 
 * @param map The map to clear.
 */
void clear_map(Map map)
{
    for (unsigned int i = 0; i < map->col; i++)
        memset(map->arr[i], 0, map->row);
}

/**
 * @brief Destroy the map.
 * 
 * @param map The map to destory.
 */
void destroy_map(Map map)
{
    for (unsigned int i = 0; i < map->col; i++)
    {
        free(map->arr[i]);
        map->arr[i] = NULL;
    }
    free(map->arr);
    map->arr = NULL;
    free(map);
}
