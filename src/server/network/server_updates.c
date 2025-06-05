/*
** EPITECH PROJECT, 2025
** src/server/network/server_updates.c
** File description:
** Server update sending functionality
*/

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "server/server.h"
#include "server/server_updates.h"

void send_position_update(client_t *client, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "ppo #%d %d %d %d\n",
        player->id, player->x, player->y, player->orientation);
    send(client->fd, response, strlen(response), 0);
}

void send_level_update(client_t *client, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "plv #%d %d\n",
        player->id, player->level);
    send(client->fd, response, strlen(response), 0);
}

void send_elevation_start(client_t *client, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pic %d %d %d #%d\n",
        player->x, player->y, player->level, player->id);
    send(client->fd, response, strlen(response), 0);
}

void send_elevation_end(client_t *client, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pie %d %d 1\n",
        player->x, player->y);
    send(client->fd, response, strlen(response), 0);
}
