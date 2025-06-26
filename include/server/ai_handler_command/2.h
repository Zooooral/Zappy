/*
** EPITECH PROJECT, 2025
** include/server/ai_handler_command/2
** File description:
** 2.h
*/

#ifndef AI_ACTIONS2_H
    #define AI_ACTIONS2_H

    #include "server/broadcast.h"
    #include "server/payloads.h"
    #include "server/server.h"
    #include "server/payloads.h"
    #include "server/resource.h"
    #include "server/broadcast.h"
    #include "server/incantation.h"
    #include <stdlib.h>

static inline void ai_action_take(server_t *server,
    client_t *client, char *cmd)
{
    player_t *p = NULL;
    int resource_id = atoi(cmd);

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
    broadcast_player_resource_update(server, p, resource_id,
        gui_payload_resource_collected);
    broadcast_message_to_guis(server, p, gui_payload_inventory);
}

static inline void ai_action_set(server_t *server, client_t *client, char *cmd)
{
    int resource_id = cmd ? atoi(cmd) : -1;
    player_t *p;
    tile_t *tile;

    if (!server || !client || !client->player ||
        resource_id < 0 || resource_id >= RESOURCE_COUNT)
        return send_response(client, "ko\n");
    p = client->player;
    tile = map_get_tile(server->game->map, p->x, p->y);
    if (!tile || p->resources[resource_id] <= 0)
        return send_response(client, "ko\n");
    p->resources[resource_id]--;
    tile->resources[resource_id]++;
    send_response(client, "ok\n");
    broadcast_player_resource_update(server, p, resource_id,
        gui_payload_resource_dropped);
    broadcast_message_to_guis(server, p, gui_payload_inventory);
}

static inline void ai_action_incantation(server_t *server,
    client_t *client, char *cmd)
{
    int result;

    (void)cmd;
    if (!server || !client || !client->player) {
        send_response(client, "ko\n");
        return;
    }
    result = try_incantation(server, client);
    if (result)
        send_response(client, "ok\n");
    else
        send_response(client, "ko\n");
}

static inline void ai_action_eject(server_t *server, client_t *client,
    char *cmd)
{
    player_t *self = client ? client->player : NULL;
    int ejected = 0;
    int dx = self ? ((self->orientation == 2) * -(self->orientation == 1)) : 0;
    int dy = self ? (self->orientation == 3) : 0;
    int width;
    int height;
    client_t *other;

    if (!server || !self)
        return send_response(client, "ko\n");
    dy = self->orientation == 1 ? -1 : dy;
    dx = self->orientation == 4 ? -1 : dx;
    width = server->game->map->width;
    height = server->game->map->height;
    for (size_t i = 0; i < server->client_count; ++i) {
        other = &server->clients[i];
        if (other != client && other->type == CLIENT_TYPE_AI &&
            other->player &&
            other->player->x == self->x && other->player->y == self->y) {
            other->player->x = (other->player->x + dx + width) % width;
            other->player->y = (other->player->y + dy + height) % height;
            ejected++;
        }
    }
    send_response(client, ejected > 0 ? "ok\n" : "ko\n");
}

static inline void ai_action_fork(server_t *server, client_t *client,
    char *cmd)
{
}

#endif
