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
    if (!server || !client || !client->player || !resource) {
        send_response(client, "ko\n");
        return;
    }
    int resource_id = atoi(resource);
    if (resource_id < 0 || resource_id >= RESOURCE_COUNT) {
        send_response(client, "ko\n");
        return;
    }
    player_t *p = client->player;
    tile_t *tile = map_get_tile(server->game->map, p->x, p->y);
    if (!tile || p->resources[resource_id] <= 0) {
        send_response(client, "ko\n");
        return;
    }
    p->resources[resource_id]--;
    tile->resources[resource_id]++;
    char *buf = NULL;
    asprintf(&buf, "ok\n");
    send_response(client, buf);
    free(buf);
    char *pdr = NULL;
    asprintf(&pdr, "pdr #%d %d\n", p->id, resource_id);
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *gui = &server->clients[i];
        if (gui->type == CLIENT_TYPE_GRAPHIC) {
            send_response(gui, pdr);
        }
    }
    free(pdr);
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
