#include <stdlib.h>
#include <stdbool.h>
#include "map.h"
#include "fatal.h"

const short directions[8][2] = {{-1, -1}, {-1, 0}, {0, -1}, {1, 1}, {1, 0}, {0, 1}, {1, -1}, {-1, 1}};

Map create_map(unsigned short col, unsigned short row)
{
    Map new_map = NULL;
    
    if ((new_map = malloc(sizeof(struct _map))) == NULL)
            FatalError("Out of space");
    if ((new_map->arr = malloc(col * sizeof(char *))) == NULL)
            FatalError("Out of space!");
    for (unsigned short i = 0; i < col; i++)
        if ((new_map->arr[i] = calloc(row, sizeof(char))) == NULL)
            FatalError("Out of space!");
    
    new_map->col = col;
    new_map->row = row;
    return new_map;
}

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
            if (times > MAX_TIMES)
                Error("Tried too many times");
        } while(has_mine(y, x, map));
        
        map->arr[y][x] = MINE;
        for (int i = 0; i < 8; i++)
        {
            short next_y = y + directions[i][0];
            short next_x = x + directions[i][1];
            if(in_map_range(next_y, next_x, map) && !has_mine(next_y, next_x, map))
                map->arr[next_y][next_x]++;
        }
    }
}

void remove_mine(Map map, short y, short x)
{
    if (!has_mine(y, x, map))
        fprintf(stderr, "No mine in (%hd, %hd)", y, x);
    for (int i = 0; i < 8; i++)
    {
        short next_y = y + directions[i][0];
        short next_x = x + directions[i][1];
        if(in_map_range(next_y, next_x, map)&& !has_mine(next_y, next_x, map))
            map->arr[next_y][next_x]--;
    }
}

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

void destroy_map(Map *mP)
{
    for (unsigned short i = 0; i < (*mP)->col; i++)
    {
        free((*mP)->arr[i]);
        (*mP)->arr[i] = NULL;
    }
    free((*mP)->arr);
    (*mP)->arr = NULL;
    free(*mP);
    *mP = NULL;
}
