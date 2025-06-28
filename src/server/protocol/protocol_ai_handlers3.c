/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_ai_handlers3.c
** File description:
** AI protocol handlers (3/3)
*/

#include "server/server.h"
#include "server/protocol_ai.h"
#include "server/vision.h"
#include "server/incantation.h"
#include "server/broadcast.h"
#include "server/time.h"
#include "server/resource.h"
#include "server/lifecycle.h"
#include "server/payloads.h"

void handle_set(server_t *server, client_t *client, const char *resource)
{
    return create_and_queue_action(server, client, resource, AI_ACTION_SET);
}

void handle_incantation(server_t *server, client_t *client, const char *arg)
{
    tile_t *tile = NULL;

    if (!server || !client || !client->player ||
        !incantation_requirements_met(server, client->player)) {
        send_response(client, "ko\n");
        return;
    }
    send_response(client, "Elevation underway\n");
    tile = get_player_tile(server, client->player);
    if (!tile) {
        send_response(client, "ko\n");
        return;
    }
    broadcast_string_message_to_guis(server, gui_payload_pic(tile,
        client->player->level, tile->players, tile->player_count));
    return create_and_queue_action(server, client, arg, AI_ACTION_INCANTATION);
}
