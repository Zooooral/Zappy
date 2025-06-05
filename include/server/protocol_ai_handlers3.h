/*
** EPITECH PROJECT, 2025
** include/server/protocol_ai_handlers3.h
** File description:
** Export handlers 11-12 for protocol_ai
*/

#ifndef PROTOCOL_AI_HANDLERS3_H
    #define PROTOCOL_AI_HANDLERS3_H

    #include "server/server.h"
    #include "server/protocol_ai.h"

void handle_set(server_t *server, client_t *client, const char *resource);
void handle_incantation(server_t *server, client_t *client, const char *arg);

#endif // PROTOCOL_AI_HANDLERS3_H
