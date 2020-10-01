#ifndef __MAP_H
#define __MAP_H

#define MINE (9)
#define EXPLODED_MINE (10)
#define EMPTY ('0')

#define MAX_TIMES (10000)

#define has_mine(y, x, map) (map->arr[y][x] == MINE)
#define has_exploded_mine(y, x, map) (map->arr[y][x] == EXPLODED_MINE)
#define in_range(y, x, up, left, down, right) (y >= up && x >= left && y < down && x < right)
#define in_map_range(y, x, map) in_range(y, x, 0, 0, map->col, map->row)
#define is_empty(y, x, map) (map->arr[y][x] == EMPTY)
#define is_shown(y, x, map) (map->arr[y][x] >= '0' && map->arr[y][x] <= '9')

/* The biggest char in map is '9'(00111001),
 * so I set flag in sixth bit. (start from zero)
 */
#define FLAG_BIT 6
#define set_flag(n) ((n) |= (1 << FLAG_BIT))
#define has_flag(n) ((n) & (1 << FLAG_BIT))
#define unset_flag(n) ((n) &= ~(1 << FLAG_BIT))

typedef struct _map{
    char **arr;
    int col, row;
} * Map;

Map create_map(unsigned short col, unsigned short row);
void put_mines(Map map, unsigned short num);
void remove_mine(Map map, short y, short x);
unsigned short cnt_mines(Map map, short y, short x);
unsigned short cnt_flags(Map map, short y, short x);
void destroy_map(Map *p_m);

#endif
