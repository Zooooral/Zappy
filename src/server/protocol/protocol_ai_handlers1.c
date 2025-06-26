/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_ai_handlers1.c
** File description:
** AI protocol handlers (1/3)
*/

#include "server/server.h"
#include "server/protocol_ai.h"
#include "server/vision.h"
#include "server/incantation.h"
#include "server/broadcast.h"
#include "server/time.h"
#include "server/resource.h"
#include "server/lifecycle.h"
#include <string.h>
#include <stdio.h>

void handle_forward(server_t *server, client_t *client, const char *arg)
{
    create_and_queue_action(server, client, "Forward", AI_ACTION_FORWARD);
}

void handle_right(server_t *server, client_t *client, const char *arg)
{
    create_and_queue_action(server, client, "Right", AI_ACTION_RIGHT);
}

void handle_left(server_t *server, client_t *client, const char *arg)
{
    create_and_queue_action(server, client, "Left", AI_ACTION_LEFT);
}

void handle_look(server_t *server, client_t *client, const char *arg)
{
    create_and_queue_action(server, client, "Look", AI_ACTION_LOOK);
}

void handle_inventory(server_t *server, client_t *client, const char *arg)
{
    create_and_queue_action(server, client, "Inventory", AI_ACTION_INVENTORY);
}
