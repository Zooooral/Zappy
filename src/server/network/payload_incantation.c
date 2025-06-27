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

const char *gui_payload_pie(const player_t *player, const tile_t *tile)
{
    char response[16];

    snprintf(response, sizeof(response), "pie %d %d %d\n",
        tile->players[0]->x, tile->players[0]->y, player->level);
    return strdup(response);
}
