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
