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


const char *gui_payload_pgt(const player_t *player, int resource_id)
{
    char response[128];

    if (!player || resource_id < 0 || resource_id >= RESOURCE_COUNT) {
        return NULL;
    }
    snprintf(response, sizeof(response), "pgt #%d %d\n", player->id, resource_id);
    return strdup(response);
}

const char *gui_payload_pdr(const player_t *player, int resource_id)
{
    char response[128];

    if (!player || resource_id < 0 || resource_id >= RESOURCE_COUNT) {
        return NULL;
    }
    snprintf(response, sizeof(response), "pdr #%d %d\n", player->id, resource_id);
    return strdup(response);
}

const char *gui_payload_pin(client_t *, const player_t *player)
{
    char response[256];

    if (!player) {
        return NULL;
    }
    snprintf(response, sizeof(response),
        "pin #%d %d %d %d %d %d %d %d %d %d\n",
        player->id, player->x, player->y,
        player->resources[RESOURCE_FOOD],
        player->resources[RESOURCE_LINEMATE],
        player->resources[RESOURCE_DERAUMERE],
        player->resources[RESOURCE_SIBUR],
        player->resources[RESOURCE_MENDIANE],
        player->resources[RESOURCE_PHIRAS],
        player->resources[RESOURCE_THYSTAME]);
    return strdup(response);
}
