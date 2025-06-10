/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_graphic.c
** File description:
** Protocol handling for graphic clients
*/

#include "server/server.h"
#include "server/protocol_graphic.h"
#include "server/server_updates.h"
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

static void format_tile_response(char *response, int x, int y,
    const tile_t *tile)
{
    snprintf(response, 128, "bct %d %d %d %d %d %d %d %d %d\n",
        x, y, tile->resources[RESOURCE_FOOD],
        tile->resources[RESOURCE_LINEMATE],
        tile->resources[RESOURCE_DERAUMERE],
        tile->resources[RESOURCE_SIBUR],
        tile->resources[RESOURCE_MENDIANE],
        tile->resources[RESOURCE_PHIRAS],
        tile->resources[RESOURCE_THYSTAME]);
}

void protocol_send_tile_content(server_t *server, client_t *client,
    int x, int y)
{
    char response[128];
    tile_t *tile;

    if (!server || !client || !server->game || !server->game->map)
        return;
    tile = map_get_tile(server->game->map, x, y);
    if (!tile) {
        send_response(client, "sbp\n");
        return;
    }
    format_tile_response(response, x, y, tile);
    send_response(client, response);
}

void protocol_send_player_info(client_t *client, const player_t *player)
{
    char response[128];

    if (!client || !player)
        return;
    snprintf(response, sizeof(response), "pnw #%d %d %d %d %d %s\n",
        player->id, player->x, player->y, player->orientation,
        player->level, player->team_name);
    send_response(client, response);
}

static void handle_map_size_command(server_t *server, client_t *client,
    const char *cmd)
{
    (void)cmd;
    protocol_send_map_size(server, client);
}

static void handle_map_content_command(server_t *server, client_t *client,
    const char *cmd)
{
    int y;
    int x;

    (void)cmd;
    for (y = 0; y < server->game->map->height; y++) {
        for (x = 0; x < server->game->map->width; x++)
            protocol_send_tile_content(server, client, x, y);
    }
}

static void handle_tile_content_command(server_t *server, client_t *client,
    const char *cmd)
{
    int x;
    int y;

    if (sscanf(cmd, "bct %d %d", &x, &y) == 2) {
        protocol_send_tile_content(server, client, x, y);
    } else {
        send_response(client, "sbp\n");
    }
}

static void handle_team_names_command(server_t *server, client_t *client,
    const char *cmd)
{
    size_t i;
    char response[64];

    (void)cmd;
    for (i = 0; i < server->config.team_count; i++) {
        snprintf(response, sizeof(response), "tna %s\n",
            server->config.team_names[i]);
        send_response(client, response);
    }
}

static void handle_player_info_command(server_t *server, client_t *client,
    const char *cmd)
{
    (void)cmd;
    if (server->game->seeder && server->game->seeder->player) {
        protocol_send_player_info(client, server->game->seeder->player);
    }
}

static void handle_position_update(server_t *server, client_t *client,
    const char *cmd)
{
    int player_id;
    player_t *player;

    sscanf(cmd, "ppo #%d", &player_id);
    player = player_find_by_id(server, player_id);
    if (player) {
        send_position_update(client, player);
    }
}

static graphic_cmd_handler_t find_graphic_handler(const char *cmd)
{
    size_t i;
    const graphic_cmd_entry_t graphic_commands[] = {
        {"msz", handle_map_size_command},
        {"mct", handle_map_content_command},
        {"bct ", handle_tile_content_command},
        {"tna", handle_team_names_command},
        {"pnw", handle_player_info_command},
        {"ppo", handle_position_update},
        {NULL, NULL}
    };

    for (i = 0; graphic_commands[i].cmd != NULL; i++) {
        if (strncmp(cmd, graphic_commands[i].cmd,
            strlen(graphic_commands[i].cmd)) == 0)
            return graphic_commands[i].handler;
    }
    return NULL;
}

void protocol_handle_graphic_command(server_t *server, client_t *client,
    const char *cmd)
{
    graphic_cmd_handler_t handler;

    if (!server || !client || !cmd)
        return;
    handler = find_graphic_handler(cmd);
    if (handler) {
        handler(server, client, cmd);
    } else {
        send_response(client, "suc\n");
    }
}
