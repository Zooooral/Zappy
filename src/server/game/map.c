/*
** EPITECH PROJECT, 2025
** src/server/game/map.c
** File description:
** Fixed map and tile management system
*/

#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "server/game.h"

static void tile_init(tile_t *tile)
{
    if (!tile)
        return;
    memset(tile, 0, sizeof(tile_t));
    tile->player_capacity = 10;
    tile->players = calloc(tile->player_capacity, sizeof(player_t *));
}

static void tile_cleanup(tile_t *tile)
{
    if (!tile)
        return;
    if (tile->players) {
        free(tile->players);
        tile->players = NULL;
    }
}

static int initialize_tile_row(map_t *map, int y)
{
    int x;

    map->tiles[y] = calloc(map->width, sizeof(tile_t));
    if (!map->tiles[y])
        return -1;
    for (x = 0; x < map->width; x++) {
        tile_init(&map->tiles[y][x]);
        if (!map->tiles[y][x].players)
            return -1;
    }
    return 0;
}

int allocate_map_tiles(map_t *map)
{
    int y;

    map->tiles = calloc(map->height, sizeof(tile_t *));
    if (!map->tiles)
        return -1;
    for (y = 0; y < map->height; y++) {
        if (initialize_tile_row(map, y) == -1)
            return -1;
    }
    return 0;
}

static void cleanup_tile_row(map_t *map, int y)
{
    int x;

    if (!map->tiles[y])
        return;
    for (x = 0; x < map->width; x++)
        tile_cleanup(&map->tiles[y][x]);
    free(map->tiles[y]);
}

static void cleanup_all_tiles(map_t *map)
{
    int y;

    if (!map->tiles)
        return;
    for (y = 0; y < map->height; y++)
        cleanup_tile_row(map, y);
    free(map->tiles);
}

void map_destroy(map_t *map)
{
    if (!map)
        return;
    cleanup_all_tiles(map);
    free(map);
}
