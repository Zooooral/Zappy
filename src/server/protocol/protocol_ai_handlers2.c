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
#include "server/payloads.h"


void handle_broadcast(server_t *server, client_t *client, const char *msg)
{
    return create_and_queue_action(server, client, msg, AI_ACTION_BROADCAST);
}

void handle_connect_nbr(server_t *server, client_t *client, const char *arg)
{
    return create_and_queue_action(server, client, arg, AI_ACTION_CONNECT_NBR);
}

void handle_fork(server_t *server, client_t *client, const char *arg)
{
    return create_and_queue_action(server, client, arg, AI_ACTION_FORK);
}

void handle_eject(server_t *server, client_t *client, const char *arg)
{
    return create_and_queue_action(server, client, arg, AI_ACTION_EJECT);
}

void handle_take(server_t *server, client_t *client, const char *resource)
{
    return create_and_queue_action(server, client, resource, AI_ACTION_TAKE);
}
