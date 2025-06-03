/*
** EPITECH PROJECT, 2025
** include/server/protocol_ai.h
** File description:
** Protocol handling for AI clients
*/

#ifndef PROTOCOL_AI_H_
    #define PROTOCOL_AI_H_

    #include "server.h"

typedef enum {
    AI_ACTION_FORWARD,
    AI_ACTION_RIGHT,
    AI_ACTION_LEFT,
    AI_ACTION_LOOK,
    AI_ACTION_INVENTORY
} ai_action_type_t;

typedef struct ai_action_data_s {
    ai_action_type_t type;
    server_t *server;
} ai_action_data_t;

void protocol_handle_ai_command(server_t *server, client_t *client,
    const char *cmd);

#endif /* !PROTOCOL_AI_H_ */
