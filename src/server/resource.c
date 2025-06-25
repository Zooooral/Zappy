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
#include "server/broadcast.h"
#include "server/server_updates.h"

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

static int count_resource_on_map(map_t *map, int type)
{
    int count = 0;
    int x;
    int y;
    tile_t *tile;

    for (y = 0; y < map->height; ++y) {
        for (x = 0; x < map->width; ++x) {
            tile = map_get_tile(map, x, y);
            if (tile)
                count += tile->resources[type];
        }
    }
    return count;
}

static void place_resource_type(server_t *server, int type)
{
    int target_quantity = ressource_quantity(server->game->map, type);
    int current_quantity = count_resource_on_map(server->game->map, type);
    int to_add = target_quantity - current_quantity;
    int x;
    int y;
    tile_t *tile;

    for (int i = 0; i < to_add; i++) {
        x = rand() % server->game->map->width;
        y = rand() % server->game->map->height;
        tile = map_get_tile(server->game->map, x, y);
        if (tile) {
            tile->resources[type]++;
            broadcast_tile_to_guis(server, x, y);
        } else {
            --i;
        }
    }
}

void respawn_resources(server_t *server)
{
    int type;

    if (!server)
        return;
    for (type = 0; type < RESOURCE_COUNT; type++)
        place_resource_type(server, type);
}
