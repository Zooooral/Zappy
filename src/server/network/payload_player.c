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

const char *gui_payload_new_player(client_t *, const player_t *player)
{
    char response[128];

    snprintf(response, sizeof(response), "pnw #%d %d %d %d %d %s\n",
        player->id, player->x, player->y, player->orientation,
        player->level, player->team_name);
    return strdup(response);
}

const char *gui_payload_position_update(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "ppo #%d %d %d %d\n",
        player->id, player->x, player->y, player->orientation);
    return strdup(response);
}

const char *gui_payload_level_update(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "plv #%d %d\n",
        player->id, player->level);
    return strdup(response);
}

const char *gui_payload_elevation_start(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pic %d %d %d #%d\n",
        player->x, player->y, player->level, player->id);
    return strdup(response);
}

const char *gui_payload_elevation_end(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pie %d %d 1\n",
        player->x, player->y);
    return strdup(response);
}
