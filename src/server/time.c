/*
** EPITECH PROJECT, 2025
** src/server/time.c
** File description:
** Time management implementation for Zappy
*/

#include <stdlib.h>
#include <stdio.h>

#include "server/time.h"
#include "server/server.h"

double get_time_unit(server_t *server)
{
    if (!server || server->config.freq == 0)
        return 1.0;
    return 1.0 / (double)server->config.freq;
}

bool set_time_unit(server_t *server, double time_unit)
{
    if (!server || time_unit <= 0.0) {
        fprintf(stderr, "Invalid time unit value: %f\n", time_unit);
        return false;
    }
    server->config.freq = (size_t)(1.0 / time_unit);
    if (server->config.freq == 0) {
        fprintf(stderr, "Frequency cannot be zero.\n");
        return false;
    }
    return true;
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

static void execute_action_callback(client_t *client,
    action_t *action, double now)
{
    (void)now;
    if (action->callback) {
        action->callback(client, action->data);
    }
}

static int process_ready_action(client_t *client, double now)
{
    action_t *action = client->action_queue_head;
    action_t *to_free;

    if (!action)
        return 0;
    if (action->exec_time > now)
        return 0;
    client->action_queue_head = action->next;
    if (!client->action_queue_head)
        client->action_queue_tail = NULL;
    client->action_queue_count--;
    execute_action_callback(client, action, now);
    to_free = action;
    free(to_free->command);
    free(to_free);
    return 1;
}

void process_actions(server_t *server)
{
    double now = get_current_time();
    client_t *client;

    for (size_t i = 0; server && i < server->client_count; ++i) {
        client = &server->clients[i];
        while (process_ready_action(client, now));
    }
}
