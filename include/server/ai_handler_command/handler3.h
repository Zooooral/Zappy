/*
** EPITECH PROJECT, 2025
** ./include/server/ai_handler_command/3
** File description:
** 3.h
*/

#ifndef AI_ACTIONS3_H
    #define AI_ACTIONS3_H

    #include <string.h>

static inline int get_team_player_count(server_t *server,
    const char *team_name)
{
    int count = 0;
    player_t *p = NULL;

    for (size_t i = 0; i < server->game->player_count; ++i) {
        p = server->game->players[i];
        if (p && p->team_name && strcmp(p->team_name, team_name) == 0)
            count++;
    }
    return count;
}

static inline void ai_action_connect_nbr(server_t *server,
    client_t *client, char *cmd)
{
    size_t available = server->config.max_clients_per_team;
    char *buf = NULL;
    int ret;
    int current;

    if (!server || !client || !client->team_name) {
        send_response(client, "ko\n");
        return (void)cmd;
    }
    current = get_team_player_count(server, client->team_name);
    available = (current > available) ? 0 : available - current;
    ret = asprintf(&buf, "%ld\n", available);
    if (ret < 0 || !buf) {
        send_response(client, "ko\n");
        return;
    }
    send_response(client, buf);
    free(buf);
}

#endif
