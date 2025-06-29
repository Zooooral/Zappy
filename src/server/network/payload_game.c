/*
** EPITECH PROJECT, 2025
** src/server/network/payload_game.c
** File description:
** Game-related GUI payload functions for Zappy
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "server/payloads.h"

char *gui_payload_game_end(int winning_team_id)
{
    char *response;

    if (asprintf(&response, "seg %d\n", winning_team_id) == -1)
        return NULL;
    return response;
}

char *gui_payload_time_unit_get(int time_unit)
{
    char *response;

    if (asprintf(&response, "sgt %d\n", time_unit) == -1)
        return NULL;
    return response;
}

char *gui_payload_time_unit_set(int time_unit)
{
    char *response;

    if (asprintf(&response, "sst %d\n", time_unit) == -1)
        return NULL;
    return response;
}
