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

void broadcast_player_death(server_t *server, player_t *player)
{
    char *payload = gui_payload_pdi(NULL, player);

    if (!server || !player || !payload)
        return;
    broadcast_string_message_to_guis(server, payload);
    free(payload);
}

void broadcast_game_end(server_t *server, int winning_team_id)
{
    char *payload = gui_payload_game_end(winning_team_id);

    if (!server || !payload)
        return;
    broadcast_string_message_to_guis(server, payload);
    free(payload);
}

void broadcast_time_unit(server_t *server)
{
    char *payload = NULL;

    if (!server)
        return;
    payload = gui_payload_time_unit_get(server->config.freq);
    if (payload) {
        broadcast_string_message_to_guis(server, payload);
        free(payload);
    }
}
