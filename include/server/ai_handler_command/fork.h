/*
** EPITECH PROJECT, 2025
** /home/vj/coding/zappy/include/server/ai_handler_command/fork
** File description:
** fork
*/

#ifndef AI_ACTION_FORK_H
    #define AI_ACTION_FORK_H


    #include "server/broadcast.h"
    #include "server/egg_manager.h"

static inline void ai_action_fork(server_t *server, client_t *client,
    char *cmd)
{
    egg_t *egg;

    (void)cmd;
    if (!server || !client || !client->player)
        return send_response(client, "ko\n");
    egg = egg_manager_add_egg(server, client->player);
    if (!egg)
        return send_response(client, "ko\n");
    broadcast_egg_laid(server, egg->id, client->player);
    send_response(client, "ok\n");
}

#endif // AI_ACTION_FORK_H
