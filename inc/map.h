#ifndef __MAP_H
#define __MAP_H

#define UNKNOWN ('#')
#define MINE ('*')
#define EMPTY (' ')

#define MAX_TIMES (10000)

#define has_mine(y, x, map) (map->arr[y][x] == MINE) 
#define in_range(y, x, up, left, down, right) (y >= up && x >= left && y < down && x < right)
#define in_map_range(y, x, map) in_range(y, x, 0, 0, map->col, map->row)

typedef struct _map{
    char **arr;
    int col, row;
} * Map;

Map create_map(unsigned short col, unsigned short row);
void put_mines(Map map, unsigned short num);
void remove_mine(Map map, short y, short x);
unsigned short cnt_mines(Map map, short m, short n);
void destroy_map(Map *mP);

#endif
