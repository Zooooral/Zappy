/*
** EPITECH PROJECT, 2025
** src/server/network/payload_egg.c
** File description:
** Egg-related GUI payload functions for Zappy
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "server/payloads.h"

char *gui_payload_egg_laid(int egg_id, const player_t *player)
{
    char *response;

    if (!player)
        return NULL;
    if (asprintf(&response, "enw #%d #%d %d %d\n",
        egg_id, player->id, player->x, player->y) == -1)
        return NULL;
    return response;
}

char *gui_payload_egg_hatched(int egg_id)
{
    char *response;

    if (asprintf(&response, "ebo #%d\n", egg_id) == -1)
        return NULL;
    return response;
}

char *gui_payload_egg_died(int egg_id)
{
    char *response;

    if (asprintf(&response, "edi #%d\n", egg_id) == -1)
        return NULL;
    return response;
}
