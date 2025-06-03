#define _GNU_SOURCE
/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_ai_handlers1.c
** File description:
** AI protocol handlers (1/3)
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
#include <stdio.h>

static void ai_action_forward(server_t *server, client_t *client)
{
    player_t *p = NULL;;
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};

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
    player_set_position(p, server->game->map,
            p->x + dx[p->orientation], p->y + dy[p->orientation]);
    send_response(client, "ok\n");
}

static void ai_action_right(server_t *server, client_t *client)
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
    if (!p) {
        send_response(client, "ko\n");
        return;
    }
    p->orientation = (p->orientation + 1) % 4;
    send_response(client, "ok\n");
}

static void ai_action_left(server_t *server, client_t *client)
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
    send_response(client, "ok\n");
}

static void ai_action_look(server_t *server, client_t *client)
{
    char *result;

    if (!server || !client)
        return;
    result = vision_look(client, server->game->map);
    if (result) {
        send_response(client, result);
        free(result);
    } else {
        send_response(client, "[]\n");
    }
}

static void ai_action_inventory(server_t *server, client_t *client)
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

static void s_ai_callback_handler(client_t *client, void *data)
{
    ai_action_data_t *action_data = (ai_action_data_t *)data;
    server_t *server;
    static const void (*action_handlers[])(server_t *, client_t *) = {
        [AI_ACTION_FORWARD] = ai_action_forward,
        [AI_ACTION_RIGHT] = ai_action_right,
        [AI_ACTION_LEFT] = ai_action_left,
        [AI_ACTION_LOOK] = ai_action_look,
        [AI_ACTION_INVENTORY] = ai_action_inventory,
    };

    if (!action_data || !client)
        return;
    server = action_data->server;
    if (action_data->type >= 0 && action_data->type <= AI_ACTION_INVENTORY &&
        action_handlers[action_data->type]) {
        action_handlers[action_data->type](server, client);
    }
    free(action_data);
}

void handle_forward(server_t *server, client_t *client, const char *arg)
{
    action_t *action;
    ai_action_data_t *data;
    double now = get_current_time();

    if (!server || !client)
        return;
    action = calloc(1, sizeof *action);
    data = calloc(1, sizeof *data);
    if (!action || !data) {
        free(action);
        return free(data);
    }
    data->type = AI_ACTION_FORWARD;
    data->server = server;
    action->command = strdup("Forward");
    action->exec_time = now + 7.0 * get_time_unit(server);
    action->callback = s_ai_callback_handler;
    action->data = data;
    queue_action(client, action);
}

void handle_right(server_t *server, client_t *client, const char *arg)
{
    action_t *action;
    ai_action_data_t *data;
    double now = get_current_time();

    if (!server || !client)
        return;
    action = calloc(1, sizeof *action);
    data = calloc(1, sizeof *data);
    if (!action || !data) {
        free(action);
        return free(data);
    }
    data->type = AI_ACTION_RIGHT;
    data->server = server;
    action->command = strdup("Right");
    action->exec_time = now + 7.0 * get_time_unit(server);
    action->callback = s_ai_callback_handler;
    action->data = data;
    queue_action(client, action);
}

void handle_left(server_t *server, client_t *client, const char *arg)
{
    action_t *action;
    ai_action_data_t *data;
    double now = get_current_time();

    if (!server || !client)
        return;
    action = calloc(1, sizeof *action);
    data = calloc(1, sizeof *data);
    if (!action || !data) {
        free(action);
        free(data);
        return;
    }
    data->type = AI_ACTION_LEFT;
    data->server = server;
    action->command = strdup("Left");
    action->exec_time = now + 7.0 * get_time_unit(server);
    action->callback = s_ai_callback_handler;
    action->data = data;
    queue_action(client, action);
}

void handle_look(server_t *server, client_t *client, const char *arg)
{
    action_t *action;
    ai_action_data_t *data;
    double now = get_current_time();

    if (!server || !client)
        return;
    action = calloc(1, sizeof *action);
    data = calloc(1, sizeof *data);
    if (!action || !data) {
        free(action);
        free(data);
        return;
    }
    data->type = AI_ACTION_LOOK;
    data->server = server;
    action->command = strdup("Look");
    action->exec_time = now + 7.0 * get_time_unit(server);
    action->callback = s_ai_callback_handler;
    action->data = data;
    action->next = NULL;
    queue_action(client, action);
}

void handle_inventory(server_t *server, client_t *client, const char *arg)
{
    action_t *action;
    ai_action_data_t *data;
    double now = get_current_time();

    if (!server || !client)
        return;
    action = calloc(1, sizeof *action);
    data = calloc(1, sizeof *data);
    if (!action || !data) {
        free(action);
        free(data);
        return;
    }
    data->type = AI_ACTION_INVENTORY;
    data->server = server;
    action->command = strdup("Inventory");
    action->exec_time = now + 1.0 * get_time_unit(server);
    action->callback = s_ai_callback_handler;
    action->data = data;
    action->next = NULL;
    queue_action(client, action);
}
