/*
** EPITECH PROJECT, 2025
** src/server/network/payloads.c
** File description:
** Server update sending functionality
*/

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "server/server.h"
#include "server/server_updates.h"

static void format_tile_response(char *response, int x, int y,
    const tile_t *tile)
{
    snprintf(response, 128, "bct %d %d %d %d %d %d %d %d %d\n",
        x, y, tile->resources[RESOURCE_FOOD],
        tile->resources[RESOURCE_LINEMATE],
        tile->resources[RESOURCE_DERAUMERE],
        tile->resources[RESOURCE_SIBUR],
        tile->resources[RESOURCE_MENDIANE],
        tile->resources[RESOURCE_PHIRAS],
        tile->resources[RESOURCE_THYSTAME]);
}

char *gui_payload_tile(server_t *server, int x, int y)
{
    char response[128];
    tile_t *tile;

    if (!server || !server->game || !server->game->map)
        return NULL;
    tile = map_get_tile(server->game->map, x, y);
    if (!tile) {
        return NULL;
    }
    format_tile_response(response, x, y, tile);
    return strdup(response);
}
