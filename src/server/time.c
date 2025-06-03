/*
** EPITECH PROJECT, 2025
** src/server/time.c
** File description:
** Time management implementation for Zappy
*/

#include <stdlib.h>
#include <stdio.h>

#include "server/time.h"

double get_time_unit(server_t *server)
{
    if (!server || server->config.freq == 0)
        return 1.0;
    return 1.0 / (double)server->config.freq;
}

void queue_action(client_t *client, action_t *action)
{
    if (!client || !action)
        return;
    action->next = NULL;
    if (!client->action_queue_tail) {
        client->action_queue_head = action;
        client->action_queue_tail = action;
    } else {
        client->action_queue_tail->next = action;
        client->action_queue_tail = action;
    }
    client->action_queue_count++;
}

void process_actions(server_t *server)
{
    double now = get_current_time();
    client_t *client;
    action_t *action;
    action_t *to_free;

    if (!server)
        return;
    for (size_t i = 0; i < server->client_count; ++i) {
        client = &server->clients[i];
        action = client->action_queue_head;
        while (action && action->exec_time <= now) {
            client->action_queue_head = action->next;
            if (!client->action_queue_head)
                client->action_queue_tail = NULL;
            client->action_queue_count--;
            if (action->callback)
                action->callback(client, action->data);
            to_free = action;
            action = action->next;
            free(to_free->command);
            free(to_free);
        }
    }
}
