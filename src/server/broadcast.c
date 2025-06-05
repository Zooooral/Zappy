/*
** EPITECH PROJECT, 2025
** src/server/broadcast.c
** File description:
** Broadcast system implementation for Zappy
*/

#include <stdio.h>

#include "server/broadcast.h"

void broadcast_message(client_t *sender, const char *msg)
{
    // TODO: Propager le message à tous les joueurs avec direction
}

void broadcast_message_to_guis(server_t *server, client_t *sender,
    void (*function)(client_t *, const player_t *))
{
    size_t i;
    client_t *client = NULL;
    player_t *player = NULL;

    if (!server)
        return;
    for (i = 0; i < server->client_count; ++i) {
        client = &server->clients[i];
        player = player_find_by_id(server, sender->fd);
        if (client->type == CLIENT_TYPE_GRAPHIC && player != NULL) {
            function(client, (const player_t *)player);
        }
    }
}
