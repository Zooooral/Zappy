/*
** EPITECH PROJECT, 2025
** src/server/resource.c
** File description:
** Resource management implementation for Zappy
*/

#include <stdio.h>
#include <assert.h>

#include "server/resource.h"
#include "server/game.h"

int take_resource(client_t *client, map_t *map, int resource_id)
{
    tile_t *tile;

    assert(client && map && resource_id >= 0 && resource_id < RESOURCE_COUNT);
    tile = map_get_tile(map, client->player->x, client->player->y);
    if (!tile || tile->resources[resource_id] <= 0) {
        printf("No resource of type %d available\n", resource_id);
        return -1;
    }
    tile->resources[resource_id]--;
    return 0;
}

static void place_resource_type(map_t *map, int type)
{
    int total_quantity = ressource_quantity(map, type);
    int i;
    int x;
    int y;
    tile_t *tile;

    for (i = 0; i < total_quantity; i++) {
        x = rand() % map->width;
        y = rand() % map->height;
        tile = map_get_tile(map, x, y);
        if (tile)
            tile->resources[type]++;
        else
            --i;
    }
}

void respawn_resources(map_t *map)
{
    int type;

    if (!map)
        return;
    for (type = 0; type < RESOURCE_COUNT; type++)
        place_resource_type(map, type);
}
