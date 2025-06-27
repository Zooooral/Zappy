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

const char *gui_payload_pic(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pic %d %d %d #%d\n",
        player->x, player->y, player->level, player->id);
    return strdup(response);
}

const char *gui_payload_pie(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pie %d %d 1\n",
        player->x, player->y);
    return strdup(response);
}
