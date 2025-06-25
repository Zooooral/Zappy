/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** map_helper
*/

#include "server/server.h"

tile_t *map_get_tile(const map_t *map, int x, int y)
{
    if (!map || x < 0 || y < 0 || x >= map->width || y >= map->height)
        return NULL;
    return &map->tiles[y][x];
}

map_t *map_create(int width, int height)
{
    map_t *map = malloc(sizeof(map_t));

    if (!map)
        return NULL;
    map->width = width;
    map->height = height;
    if (allocate_map_tiles(map) == -1) {
        map_destroy(map);
        return NULL;
    }
    return map;
}
