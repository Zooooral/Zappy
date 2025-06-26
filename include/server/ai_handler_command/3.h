/*
** EPITECH PROJECT, 2025
** ./include/server/ai_handler_command/3
** File description:
** 3.h
*/

#ifndef AI_ACTIONS3_H
    #define AI_ACTIONS3_H
    #include <string.h>

static inline void ai_action_connect_nbr(server_t *server,
    client_t *client, char *cmd)
{
    size_t available = server->config.max_clients_per_team;
    int current = 0;
    char *buf = NULL;
    player_t *p = NULL;

    if (!server || !client || !client->team_name) {
        send_response(client, "ko\n");
        return (void)cmd;
    }
    for (size_t i = 0; i < server->game->player_count; ++i) {
        p = server->game->players[i];
        if (p && p->team_name && strcmp(p->team_name, client->team_name) == 0)
            current++;
    }
    available = (current > available) ? 0 : available - current;
    asprintf(&buf, "%d\n", available);
    send_response(client, buf);
    free(buf);
}

#endif
