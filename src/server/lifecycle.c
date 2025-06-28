/*
** EPITECH PROJECT, 2025
** src/server/lifecycle.c
** File description:
** Player lifecycle management implementation for Zappy
*/

#include <stdio.h>
#include <stdlib.h>

#include "server/lifecycle.h"
#include "server/broadcast.h"
#include "server/payloads.h"

void consume_food(client_t *client)
{
    (void)client;
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
    send_response(client, "dead\n");
}

void fork_player(client_t *client, map_t *map)
{
    (void)client;
    (void)map;
}

