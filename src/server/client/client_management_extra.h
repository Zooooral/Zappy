/*
** EPITECH PROJECT, 2025
** /home/vj/coding/zappy/src/server/client/client_management_extra
** File description:
** eu
*/

#ifndef CLIENT_MANAGEMENT_EXTRA_H
    #define CLIENT_MANAGEMENT_EXTRA_H

// add some egg stuff here
static int count_players_in_team(server_t *server, const char *team_name)
{
    int count = 0;
    player_t *p;

    for (size_t i = 0; i < server->game->player_count; ++i) {
        p = server->game->players[i];
        if (p && p->team_name && strcmp(p->team_name, team_name) == 0)
            count++;
    }
    return count;
}

//note tODO handle unlimited guis and all that shit
static bool can_client_connect(server_t *server, client_t *client)
{
    int available;
    int current;
    bool team_found = false;

    if (!server || !client || !client->team_name ||
        server->game->player_count >= server->game->player_capacity)
        return false;
    for (size_t i = 0; i < server->config.team_count; ++i) {
        if (strcmp(client->team_name, server->config.team_names[i]) == 0) {
            current = count_players_in_team(server, client->team_name);
            available = (int)server->config.max_clients_per_team - current;
            team_found = available > 0;
            break;
        }
    }
    client->is_authenticated = true;
    return team_found;
}

static bool try_regular_connect(server_t *server, client_t *client,
    const char *message)
{
    int p[2] = {rand() % server->config.width, rand() % server->config.height};

    if (!server || !client || !client->team_name ||
        server->game->player_count >= server->game->player_capacity ||
        !can_client_connect(server, client))
        return false;
    client->type = CLIENT_TYPE_AI;
    client->player = player_create(client, p[0], p[1], message);
    if (client->player) {
        player_set_position(server, client->player, p[0], p[1]);
        add_player_to_game(server->game, client->player);
        broadcast_message_to_guis(server, client->player, gui_payload_pnw);
    }
    return false;
}

static bool hatch_from_egg(server_t *server, client_t *client,
    egg_t *egg)
{
    if (!server || !client || !egg || egg->hatched || egg->connected)
        return false;
    client->player = player_create(client, egg->x, egg->y, egg->team_name);
    if (!client->player)
        return false;
    player_set_position(server, client->player, egg->x, egg->y);
    add_player_to_game(server->game, client->player);
    broadcast_message_to_guis(server, client->player, gui_payload_pnw);
    egg->connected = true;
    broadcast_egg_hatched(server, egg->id);
    egg_manager_remove_egg(server, egg);
    egg_die(server, egg);
    return true;
}
static void client_validate(server_t *server, client_t *client,
    const char *message)
{
    char res[1024];

    client->team_name = strndup(message, strlen(message) - 1);
    client->type = CLIENT_TYPE_AI;
    if (!try_regular_connect(server, client, message) &&
        !hatch_from_egg(server, client,
            egg_manager_find_available_egg(server, message)))
        return send_response(client, "ko\n");
    snprintf(res, sizeof res, "%ld\n", server->config.max_clients_per_team);
    send_response(client, res);
    snprintf(res, sizeof(res), "%ld %ld\n", server->config.width,
        server->config.height);
    send_response(client, res);
}
#endif // CLIENT_MANAGEMENT_EXTRA_H
