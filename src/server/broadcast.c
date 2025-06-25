/*
** EPITECH PROJECT, 2025
** src/server/broadcast.c
** File description:
** Broadcast system implementation for Zappy
*/

#include <stdio.h>

#include "server/broadcast.h"
#include "server/server_updates.h"
#include "server/payloads.h"

void broadcast_message(client_t *sender, const char *msg)
{
    // TODO: Propager le message à tous les joueurs avec direction
}

void broadcast_message_to_guis(server_t *server, player_t *player,
    const char *(*function)(client_t *, const player_t *))
{
    client_t *client = NULL;
    const char *payload = function(client, player);

    if (!server || !player || !function)
        return;
    for (size_t i = 0; i < server->client_count; ++i) {
        client = &server->clients[i];
        if (client->type == CLIENT_TYPE_GRAPHIC) {
            send_response(client, payload);
        }
    }
    free((void *)payload);
}

void broadcast_tile_to_guis(server_t *server, int x, int y)
{
    client_t *client = NULL;
    char *payload = gui_payload_tile(server, x, y);

    if (!server || !payload)
        return;
    for (size_t i = 0; i < server->client_count; ++i) {
        client = &server->clients[i];
        if (client->type == CLIENT_TYPE_GRAPHIC) {
            send_response(client, payload);
        }
    }
    free(payload);
}

void broadcast_player_resource_update(server_t *server, player_t *player,
    int resource_id, char *(*function)(const player_t *, int))
{
    client_t *client = NULL;
    char *payload = function(player, resource_id);

    if (!server || !payload)
        return;
    for (size_t i = 0; i < server->client_count; ++i) {
        client = &server->clients[i];
        if (client->type == CLIENT_TYPE_GRAPHIC) {
            send_response(client, payload);
        }
    }
    free(payload);
}
