/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_ai.c
** File description:
** Protocol handling for AI clients
*/

#include "server/server.h"
#include "server/protocol_ai.h"
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

void protocol_handle_ai_command(server_t *server, client_t *client,
    const char *cmd)
{
    if (!server || !client || !cmd)
        return;
    send_response(client, "ok\n");
}
