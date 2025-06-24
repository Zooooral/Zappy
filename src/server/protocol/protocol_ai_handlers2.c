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

void handle_broadcast(server_t *server, client_t *client, const char *msg)
{
    return;
}

void handle_connect_nbr(server_t *server, client_t *client, const char *arg)
{
    if (!server || !client || !client->team_name) {
        send_response(client, "ko\n");
        return;
    }
    int available = (int)server->config.max_clients_per_team;
    int current = 0;
    for (size_t i = 0; i < server->game->player_count; ++i) {
        player_t *p = server->game->players[i];
        if (p && p->team_name && strcmp(p->team_name, client->team_name) == 0)
            current++;
    }
    available -= current;
    if (available < 0) available = 0;
    char *buf = NULL;
    asprintf(&buf, "%d\n", available);
    send_response(client, buf);
    free(buf);
}

void handle_fork(server_t *server, client_t *client, const char *arg)
{
    return;
}

void handle_eject(server_t *server, client_t *client, const char *arg)
{
    return;
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
    return;
}
