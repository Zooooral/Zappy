/*
** EPITECH PROJECT, 2025
** src/server/command_handler.c
** File description:
** Graphic client command handling functions
*/

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "server/command_handler.h"

static void send_map_size(server_t *server, client_t *client)
{
    char response[64];

    snprintf(response, sizeof(response), "msz %zu %zu\n",
        server->config.width, server->config.height);
    send(client->fd, response, strlen(response), 0);
}

static void send_tile_content(server_t *server, client_t *client, int x, int y)
{
    char response[128];

    (void)server;
    snprintf(response, sizeof(response), "bct %d %d 2 1 0 3 1 1 1\n", x, y);
    send(client->fd, response, strlen(response), 0);
}

void send_all_tiles(server_t *server, client_t *client)
{
    for (size_t y = 0; y < server->config.height; y++) {
        for (size_t x = 0; x < server->config.width; x++) {
            send_tile_content(server, client, x, y);
        }
    }
}

static void send_team_names(server_t *server, client_t *client)
{
    char response[64];

    for (size_t i = 0; i < server->config.team_count; i++) {
        snprintf(response, sizeof(response), "tna %s\n",
            server->config.team_names[i]);
        send(client->fd, response, strlen(response), 0);
    }
}

static void handle_map_size_command(server_t *server, client_t *client)
{
    send_map_size(server, client);
}

static void handle_map_content_command(server_t *server, client_t *client)
{
    send_all_tiles(server, client);
}

static void handle_tile_content_command(server_t *server, client_t *client,
    const char *cmd)
{
    int x;
    int y;

    if (sscanf(cmd, "bct %d %d", &x, &y) == 2)
        send_tile_content(server, client, x, y);
}

static void handle_team_names_command(server_t *server, client_t *client)
{
    send_team_names(server, client);
}

static void handle_unknown_command(client_t *client)
{
    send(client->fd, "suc\n", 4, 0);
}

void command_handle_graphic(server_t *server, client_t *client,
    const char *cmd)
{
    if (strncmp(cmd, "msz", 3) == 0) {
        handle_map_size_command(server, client);
        return;
    }
    if (strncmp(cmd, "mct", 3) == 0) {
        handle_map_content_command(server, client);
        return;
    }
    if (strncmp(cmd, "bct ", 4) == 0) {
        handle_tile_content_command(server, client, cmd);
        return;
    }
    if (strncmp(cmd, "tna", 3) == 0) {
        handle_team_names_command(server, client);
        return;
    }
    handle_unknown_command(client);
}
