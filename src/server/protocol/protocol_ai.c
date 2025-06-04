/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_ai.c
** File description:
** Protocol handling for AI clients
*/

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "server/server.h"
#include "server/protocol_ai.h"
#include "server/vision.h"
#include "server/incantation.h"
#include "server/broadcast.h"
#include "server/time.h"
#include "server/resource.h"
#include "server/lifecycle.h"
#include "server/gui_notify.h"
#include "server/ai_commands.h"
#include "server/protocol_ai_handlers2.h"
#include "server/protocol_ai_handlers3.h"

void protocol_handle_ai_command(server_t *server, client_t *client, const char *cmd)
{
    printf("AI command: %s\n", cmd);
    if (!server || !client || !cmd) {
        return;
    }
    for (size_t i = 0; i < ai_commands_count; ++i) {
        if (strncmp(cmd, ai_commands[i].name, ai_commands[i].len) != 0) {
            continue;
        }
        if (ai_commands[i].fn == handle_broadcast
            || ai_commands[i].fn == handle_take
            || ai_commands[i].fn == handle_set)
            ai_commands[i].fn(server, client, cmd + ai_commands[i].len);
        else
            ai_commands[i].fn(server, client, NULL);
        return;
    }
    send_response(client, "ko\n");
}

void create_and_queue_action(server_t *server, client_t *client,
    const char *cmd, const ai_action_type_t type)
{
    action_t *action;
    ai_action_data_t *data;
    double now = get_current_time();

    if (!server || !client)
        return;
    action = calloc(1, sizeof *action);
    data = calloc(1, sizeof *data);
    if (!action || !data) {
        free(action);
        return free(data);
    }
    data->type = type;
    data->server = server;
    action->command = strdup(cmd);
    action->exec_time = now + 7.0 * get_time_unit(server);
    action->callback = ai_callback_handler;
    action->data = data;
    queue_action(client, action);
}

