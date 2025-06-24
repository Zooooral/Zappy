/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_ai_handlers2.c
** File description:
** AI protocol handlers (2/3)
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
#include <string.h>


void handle_broadcast(server_t *server, client_t *client, const char *msg)
{
    return create_and_queue_action(server, client, msg, AI_ACTION_BROADCAST);
}

void handle_connect_nbr(server_t *server, client_t *client, const char *arg)
{
    return create_and_queue_action(server, client, "ConnectNbr", AI_ACTION_CONNECT_NBR);
}

void handle_fork(server_t *server, client_t *client, const char *arg)
{
    return;
}

void handle_eject(server_t *server, client_t *client, const char *arg)
{
    if (!server || !client || !client->player) {
        send_response(client, "ko\n");
        return;
    }
    int ejected = 0;
    player_t *self = client->player;
    int dx = 0, dy = 0;
    switch (self->orientation) {
        case 1: dy = -1; break; // North
        case 2: dx = 1; break;  // East
        case 3: dy = 1; break;  // South
        case 4: dx = -1; break; // West
        default: break;
    }
    int width = server->game->map->width;
    int height = server->game->map->height;
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *other = &server->clients[i];
        if (other != client && other->type == CLIENT_TYPE_AI && other->player &&
            other->player->x == self->x && other->player->y == self->y) {
            other->player->x = (other->player->x + dx + width) % width;
            other->player->y = (other->player->y + dy + height) % height;
            ejected++;
        }
    }
    char *buf = NULL;
    asprintf(&buf, "%s\n", ejected > 0 ? "ok" : "ko");
    send_response(client, buf);
    free(buf);
}

void handle_take(server_t *server, client_t *client, const char *resource)
{
    player_t *p = NULL;
    int resource_id = atoi(resource);

    if (!server || !client)
        return;
    for (int i = 0; i < server->game->player_count; ++i) {
        if (server->game->players[i]->id == client->fd) {
            p = server->game->players[i];
            break;
        }
    }
    if (!p || take_resource(client, server->game->map, resource_id) == -1) {
        return send_response(client, "ko\n");
    }
    ++p->resources[resource_id];
    send_response(client, "ok\n");
    char *pgt = NULL;
    asprintf(&pgt, "pgt #%d %d\n", p->id, resource_id);
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *gui = &server->clients[i];
        if (gui->type == CLIENT_TYPE_GRAPHIC) {
            send_response(gui, pgt);
        }
    }
    free(pgt);
    return;
}
