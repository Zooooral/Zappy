/*
** EPITECH PROJECT, 2025
** include/server/protocol_ai.h
** File description:
** Protocol handling for AI clients
*/

#ifndef PROTOCOL_AI_H_
    #define PROTOCOL_AI_H_

    #include "server.h"

void protocol_handle_ai_command(server_t *server, client_t *client,
    const char *cmd);

#endif /* !PROTOCOL_AI_H_ */
