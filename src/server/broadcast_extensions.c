/*
** EPITECH PROJECT, 2025
** src/server/broadcast_extensions.c
** File description:
** Additional broadcast implementations for Zappy
*/

#include <stdio.h>
#include <stdlib.h>

#include "server/broadcast.h"
#include "server/payloads.h"
#include "server/server.h"

void broadcast_time_unit_modified(server_t *server, int time_unit)
{
    char *payload = gui_payload_time_unit_set(time_unit);

    if (!server || !payload)
        return;
    broadcast_string_message_to_guis(server, payload);
    free(payload);
}

void broadcast_egg_laid(server_t *server, int egg_id, player_t *player)
{
    char *payload = gui_payload_egg_laid(egg_id, player);

    if (!server || !player || !payload)
        return;
    broadcast_string_message_to_guis(server, payload);
    free(payload);
}

void broadcast_egg_hatched(server_t *server, int egg_id)
{
    char *payload = gui_payload_egg_hatched(egg_id);

    if (!server || !payload)
        return;
    broadcast_string_message_to_guis(server, payload);
    free(payload);
}

void broadcast_egg_died(server_t *server, int egg_id)
{
    char *payload = gui_payload_egg_died(egg_id);

    if (!server || !payload)
        return;
    broadcast_string_message_to_guis(server, payload);
    free(payload);
}
