/*
** EPITECH PROJECT, 2025
** src/server/lifecycle.c
** File description:
** Player lifecycle management implementation for Zappy
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "server/lifecycle.h"
#include "server/broadcast.h"
#include "server/payloads.h"

void consume_food(client_t *client)
{
    player_t *player = NULL;

    if (!client || !client->player)
        return;
    player = client->player;
    if (player->resources[RESOURCE_FOOD] > 0) {
        player->resources[RESOURCE_FOOD]--;
    } else {
        if (client->player->client) {
            player_die(client, NULL);
        }
    }
}

void player_die(client_t *client, server_t *server)
{
    player_t *player = client ? client->player : NULL;
    tile_t *tile = NULL;

    if (!client || !player)
        return;
    player->is_alive = false;
    if (server && server->game) {
        broadcast_player_death(server, player);
        tile = map_get_tile(server->game->map, player->x, player->y);
        assert(tile != NULL);
        for (int i = 0; i < RESOURCE_COUNT; i++) {
            tile->resources[i] += player->resources[i];
            player->resources[i] = 0;
        }
        broadcast_tile_to_guis(server, player->x, player->y);
    }
    send_response(client, "dead\n");
}

void fork_player(client_t *client, map_t *map)
{
    (void)client;
    (void)map;
}
