/**
 * @file map.c
 * @author jkilopu
 * @brief Provides functions for the creation and manipulation of the 2d map and its blocks.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "map.h"
#include "fatal.h"

#define FAILED_PLACEMENT_MAX_TIMES (10000)

const short directions[8][2] = {{-1, -1}, {-1, 0}, {0, -1}, {1, 1}, {1, 0}, {0, 1}, {1, -1}, {-1, 1}};
extern FILE *output;

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
Map create_map(unsigned short col, unsigned short row)
{
    Map new_map = NULL;
    
    new_map = malloc_fatal(sizeof(struct _map), "create_map - new_map");
    new_map->arr = malloc_fatal(col * sizeof(char *), "create_map - new_map->arr");
    for (unsigned short i = 0; i < col; i++)
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
void put_mines(Map map, unsigned short num)
{
    short y, x;
    int times = 0;
    
    while(num--)
    {
        do
        {
            y = rand() % map->col;
            x = rand() % map->row;
            times++;
            if (times > FAILED_PLACEMENT_MAX_TIMES)
                Error("Tried too many times");
        } while(has_mine(y, x, map));
        
        set_mine(y, x, map);
        for (int i = 0; i < 8; i++)
        {
            short next_y = y + directions[i][0];
            short next_x = x + directions[i][1];
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
void remove_mine(Map map, short y, short x)
{
    if (!has_mine(y, x, map))
    {
        fprintf(stderr, "No mine in (%hd, %hd)", y, x);
        exit(1);
    }
    for (int i = 0; i < 8; i++)
    {
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
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
unsigned short cnt_mines(Map map, short y, short x)
{
    unsigned short cnt = 0;
    for (int i = 0; i < 8; i++)
    {
            short next_y = y + directions[i][0];
            short next_x = x + directions[i][1];
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
unsigned short cnt_flags(Map map, short y, short x)
{
    unsigned short cnt = 0;
    for (int i = 0; i < 8; i++)
    {
            short next_y = y + directions[i][0];
            short next_x = x + directions[i][1];
            if(in_map_range(next_y, next_x, map) && has_flag(next_y, next_x, map))
                cnt++;
    }
    return cnt;
}

/**
 * @brief Clear the map.
 * 
 * @param map The map to clear.
 * 
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
    for (unsigned short i = 0; i < map->col; i++)
    {
        free(map->arr[i]);
        map->arr[i] = NULL;
    }
    free(map->arr);
    map->arr = NULL;
    free(map);
}
