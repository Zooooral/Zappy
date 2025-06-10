/*
** EPITECH PROJECT, 2025
** include/server/protocol_ai_handlers2.h
** File description:
** Export handlers 6-10 for protocol_ai
*/

#ifndef PROTOCOL_AI_HANDLERS2_H
    #define PROTOCOL_AI_HANDLERS2_H

    #include "server/server.h"
    #include "server/protocol_ai.h"

void handle_broadcast(server_t *server, client_t *client, const char *msg);
void handle_connect_nbr(server_t *server, client_t *client, const char *arg);
void handle_fork(server_t *server, client_t *client, const char *arg);
void handle_eject(server_t *server, client_t *client, const char *arg);
void handle_take(server_t *server, client_t *client, const char *resource);

#endif // PROTOCOL_AI_HANDLERS2_H
