/*
** EPITECH PROJECT, 2025
** Zappy
** File description:
** protocol_graphic_helper
*/

#include "server/server.h"
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

void send_response(client_t *client, const char *response)
{
    ssize_t sent;

    if (!client || !response)
        return;
    sent = send(client->fd, response, strlen(response), 0);
    printf("[SERVER] Sent response to client %d: %s", client->fd, response);
    (void)sent;
}

void protocol_send_map_size(server_t *server, client_t *client)
{
    char response[64];

    if (!server || !client || !server->game || !server->game->map)
        return;
    snprintf(response, sizeof(response), "msz %d %d\n",
        server->game->map->width, server->game->map->height);
    send_response(client, response);
}
