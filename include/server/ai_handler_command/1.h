#ifndef AI_ACTIONS_H
#define AI_ACTIONS_H

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
    player_set_position(p, server->game->map,
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
    send_response(client, "ok\n");
}


static inline void ai_action_look(server_t *server, client_t *client)
{
    char *result;

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

#endif
