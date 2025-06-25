/*
** EPITECH PROJECT, 2025
** /home/vj/coding/B-YEP-400-PAR-4-1-zappy-maxence.bunel/src/server/protocol/ai_callback
** File description:
** ai callbacks (after the ticks passed)
*/

#include "server/dynamic_array.h"
#include "server/server.h"
#include "server/protocol_ai.h"
#include "server/server_updates.h"

static inline void ai_action_forward(server_t *server, client_t *client)
{
    player_t *p = NULL;
    static const int dx[4] = {0, 1, 0, -1};
    static const int dy[4] = {-1, 0, 1, 0};

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

static inline void ai_action_right(server_t *server, client_t *client)
{
    size_t i;
    player_t *p;

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
    p->orientation = (p->orientation + 1) % 4;
    player_set_position(server, p, p->x, p->y);
    send_response(client, "ok\n");
}

static inline void ai_action_left(server_t *server, client_t *client)
{
    size_t i;
    player_t *p = NULL;

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
    p->orientation = (p->orientation + 3) % 4;
    player_set_position(server, p, p->x, p->y);
    send_response(client, "ok\n");
}


static inline void ai_action_look(server_t *server, client_t *client)
{
    char *result;

    if (!server || !client)
        return;
    // result = vision_look(client, server->game->map);
    // if (result) {
    //     send_response(client, result);
    //     da_destroy(result);
    // } else {
        send_response(client, "[]\n");
    // }
}

static inline void ai_action_inventory(server_t *server, client_t *client)
{
    size_t i;
    player_t *p = NULL;
    char *buf;

    if (!server || !client)
        return;
    for (i = 0; i < server->game->player_count; ++i)
        if (server->game->players[i]->id == client->fd) {
            p = server->game->players[i];
            break;
        }
    if (!p)
        return send_response(client, "ko\n");
    asprintf(&buf, "[food %d, linemate %d, "
        "deraumere %d, sibur %d, mendiane %d, phiras %d, thystame %d]\n",
        p->resources[RESOURCE_FOOD], p->resources[RESOURCE_LINEMATE],
        p->resources[RESOURCE_DERAUMERE], p->resources[RESOURCE_SIBUR],
        p->resources[RESOURCE_MENDIANE], p->resources[RESOURCE_PHIRAS],
        p->resources[RESOURCE_THYSTAME]);
    send_response(client, buf);
}

static const void (*action_handlers[])(server_t *, client_t *) = {
    [AI_ACTION_FORWARD] = ai_action_forward,
    [AI_ACTION_RIGHT] = ai_action_right,
    [AI_ACTION_LEFT] = ai_action_left,
    [AI_ACTION_LOOK] = ai_action_look,
    [AI_ACTION_INVENTORY] = ai_action_inventory,
};

void ai_callback_handler(client_t *client, void *data)
{
    ai_action_data_t *action_data = (ai_action_data_t *)data;
    server_t *server;

    if (!action_data || !client)
        return;
    server = action_data->server;
    if (action_data->type >= 0 && action_data->type <= AI_ACTION_INVENTORY &&
        action_handlers[action_data->type]) {
        action_handlers[action_data->type](server, client);
    }
    free(action_data);
}
