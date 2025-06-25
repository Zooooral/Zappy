#ifndef AI_ACTIONS2_H
    #define AI_ACTIONS2_H

    #include "server/broadcast.h"
    #include "server/payloads.h"
    #include "server/server.h"
    #include "server/payloads.h"
    #include "server/resource.h"
    #include "server/broadcast.h"
    #include <stdlib.h>

static inline void ai_action_take(server_t *server, client_t *client, char *cmd)
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
    char *pgt = NULL;
    asprintf(&pgt, "pgt #%d %d\n", p->id, resource_id);
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *gui = &server->clients[i];
        if (gui->type == CLIENT_TYPE_GRAPHIC) {
            send_response(gui, pgt);
        }
    }
    broadcast_player_resource_update(server, p, resource_id,
        gui_payload_resource_collected);
    free(pgt);
}

static inline void ai_action_set(server_t *server, client_t *client, char *cmd)
{
    if (!server || !client || !client->player || !cmd) {
        send_response(client, "ko\n");
        return;
    }
    int resource_id = atoi(cmd);
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
}

static inline void ai_action_incantation(server_t *server, client_t *client, char *cmd)
{
}

static inline void ai_action_eject(server_t *server, client_t *client, char *cmd)
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

static inline void ai_action_fork(server_t *server, client_t *client, char *cmd)
{
}

#endif
