/**
 * @file map.h
 * @author jkilopu
 * @brief Raw map which is independent with GUI, supports the creation and basic manupulation of the 2d map and its blocks.
 * 
 * @details About Map in mymines:
 * Map Coordinate System:
 *       (0, 0)|-----------→ x axis [0, row)
 *             |
 *             |
 *             |
 *             |
 *             ↓  y axis [0, col)
 * 
 * Map Blocks Value:
 *      0 ~ 8        The value represents the number of mines around the closed block.
 *      '0' ~ '8'    The value represents the number of mines around the opened block.
 *      9            The mine.
 *      10           The exploded mine.
 * 
 * Map Blocks Flag:
 *      The flag bit is at 6th bit of the block. For details, see "Flag Manipulate Macros" below.
 */

#ifndef __MAP_H
#define __MAP_H

//-------------------------------------------------------------------
// Map Block Type Macros
//-------------------------------------------------------------------

#define MINE (9)
#define EXPLODED_MINE (10)

//-------------------------------------------------------------------
// Map Block Manipulate Macros
//-------------------------------------------------------------------

#define set_mine(y, x, map) map->arr[y][x] = MINE
#define set_exploded_mine(y, x, map) map->arr[y][x] = EXPLODED_MINE
#define open_block(y, x, map) map->arr[y][x] += '0'
#define set_num(y, x, map, n) map->arr[y][x] = n
#define get_block(y, x, map) map->arr[y][x]

//-------------------------------------------------------------------
// Map Block Status Macros
//-------------------------------------------------------------------

#define has_mine(y, x, map) (map->arr[y][x] == MINE)
#define in_range(y, x, down, right) (y < down && x < right)
#define in_map_range(y, x, map) in_range(y, x, map->col, map->row)
#define is_empty(y, x, map) (map->arr[y][x] == '0')
#define is_num(y, x, map) (map->arr[y][x] >= 0 && map->arr[y][x] <= 8)
#define is_shown_num(y, x, map) (map->arr[y][x] >= '0' && map->arr[y][x] <= '8')
#define is_exploded_mine(y, x, map) (map->arr[y][x] == EXPLODED_MINE)
#define get_mine_num(y, x, map) (map->arr[y][x] - '0') ///< Should only used for opened blocks

/**
 * @name The position of flag bit in map block
 * 
 * @note The biggest char in map is '9'(00111001),
 *       so I set flag in sixth bit. (start from zero)
 */
#define FLAG_BIT 6

//-------------------------------------------------------------------
// Flag Manipulate Macros
//-------------------------------------------------------------------

#define set_flag(y, x, map) ((map->arr[y][x]) |= (1 << FLAG_BIT))
#define has_flag(y, x, map) ((map->arr[y][x]) & (1 << FLAG_BIT))
#define unset_flag(y, x, map) ((map->arr[y][x]) &= ~(1 << FLAG_BIT))

//-------------------------------------------------------------------
// Type Definations
//-------------------------------------------------------------------

/**
 * @brief Used to record map status.
 */
typedef struct _map {
    char **arr;                 ///< The two dimension array.
    unsigned int col, row;    ///< The column and row of the map.
} *Map;

//-------------------------------------------------------------------
// Prototypes
//-------------------------------------------------------------------

Map create_map(unsigned int col, unsigned int row);
void put_mines(Map map, unsigned int num);
void remove_mine(Map map, unsigned int y, unsigned int x);
unsigned int cnt_mines(Map map, unsigned int y, unsigned int x);
unsigned int cnt_flags(Map map, unsigned int y, unsigned int x);
void unhidden_map(Map map);
void clear_map(Map map);
void destroy_map(Map map);

#endif
