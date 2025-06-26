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
#include "server/gui_notify.h"

void handle_set(server_t *server, client_t *client, const char *resource)
{
    return create_and_queue_action(server, client, resource, AI_ACTION_SET);
}

void handle_incantation(server_t *server, client_t *client, const char *arg)
{
    if (!server || !client || !client->player ||
        !incantation_requirements_met(server, client->player)) {
        send_response(client, "ko\n");
        return;
    }
    return create_and_queue_action(server, client, arg, AI_ACTION_INCANTATION);
}
