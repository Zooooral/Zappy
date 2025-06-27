/*
** EPITECH PROJECT, 2025
** ./include/server/ai_handler_command/1
** File description:
** 1.h
*/

#ifndef AI_ACTIONS_H
    #define AI_ACTIONS_H

    #include <stdint.h>

    #include "server/server.h"
    #include "server/dynamic_array.h"
    #include "server/vision.h"

static inline void ai_action_forward(server_t *server, client_t *client,
    char *cmd)
{
    player_t *p = NULL;
    static const int dx[5] = {0, 0, 1, 0, -1};
    static const int dy[5] = {0, -1, 0, 1, 0};

    (void)cmd;
    if (!server || !client)
        return;
    for (size_t i = 0; i < server->game->player_count; ++i) {
        if (server->game->players[i]->id == client->fd) {
            p = server->game->players[i];
            break;
        }
    }
    if (!p)
        return send_response(client, "ko\n");
    player_set_position(server, p,
            p->x + dx[p->orientation], p->y + dy[p->orientation]);
    send_response(client, "ok\n");
}

static inline void ai_action_right(server_t *server, client_t *client,
    char *cmd)
{
    size_t i;
    player_t *p;

    (void)cmd;
    if (!server || !client)
        return;
    p = NULL;
    for (i = 0; i < server->game->player_count; ++i) {
        if (server->game->players[i]->id == client->fd) {
            p = server->game->players[i];
            break;
        }
    }
    if (!p)
        return send_response(client, "ko\n");
    p->orientation = (p->orientation % 4) + 1;
    player_set_position(server, p, p->x, p->y);
    send_response(client, "ok\n");
}

static inline void ai_action_left(server_t *server, client_t *client,
    char *cmd)
{
    size_t i;
    player_t *p = NULL;

    (void)cmd;
    if (!server || !client)
        return;
    for (i = 0; i < server->game->player_count; ++i) {
        if (server->game->players[i]->id == client->fd) {
            p = server->game->players[i];
            break;
        }
    }
    if (!p)
        return send_response(client, "ko\n");
    p->orientation = (p->orientation == 1) ? 4 : p->orientation - 1;
    player_set_position(server, p, p->x, p->y);
    send_response(client, "ok\n");
}

static inline void ai_action_look(server_t *server, client_t *client,
    char *cmd)
{
    char *result;

    (void)cmd;
    if (!server || !client)
        return;
    result = vision_look(client, server->game->map);
    if (result) {
        send_response(client, result);
        da_destroy(result);
    } else {
        send_response(client, "[]\n");
    }
}

static inline player_t *find_player_by_fd(server_t *server, int fd)
{
    size_t i;

    if (!server)
        return NULL;
    for (i = 0; i < server->game->player_count; ++i) {
        if (server->game->players[i]->id == fd)
            return server->game->players[i];
    }
    return NULL;
}

static inline int format_inventory_string(char **buf, player_t *p)
{
    if (!p)
        return -1;
    return asprintf(buf, "[food %d, linemate %d, deraumere %d, sibur %d, "
        "mendiane %d, phiras %d, thystame %d]\n",
        p->resources[RESOURCE_FOOD], p->resources[RESOURCE_LINEMATE],
        p->resources[RESOURCE_DERAUMERE], p->resources[RESOURCE_SIBUR],
        p->resources[RESOURCE_MENDIANE], p->resources[RESOURCE_PHIRAS],
        p->resources[RESOURCE_THYSTAME]);
}

static inline void ai_action_inventory(server_t *server, client_t *client,
    char *cmd)
{
    player_t *p;
    char *buf;
    int ret;

    (void)cmd;
    if (!server || !client)
        return;
    p = find_player_by_fd(server, client->fd);
    if (!p) {
        send_response(client, "ko\n");
        return;
    }
    ret = format_inventory_string(&buf, p);
    if (ret < 0 || !buf) {
        send_response(client, "ko\n");
        return;
    }
    send_response(client, buf);
    free(buf);
}

#endif
