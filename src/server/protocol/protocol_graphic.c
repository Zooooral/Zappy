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

    if (!client || !player) {
        printf("[DEBUG] protocol_send_player_info: client=%p player=%p\n", (void*)client, (void*)player);
        return;
    }
    snprintf(response, sizeof(response), "pnw #%d %d %d %d %d %s\n",
        player->id, player->x, player->y, player->orientation,
        player->level, player->team_name);
    send_response(client, response);
}

static void send_existing_players_after_map(server_t *server, client_t *client)
{
    static bool players_sent[1024] = {false};
    
    if (client->fd >= 1024 || players_sent[client->fd])
        return;
    for (size_t i = 0; i < server->client_count; i++) {
        if (server->clients[i].type == CLIENT_TYPE_AI && 
            server->clients[i].player != NULL) {
            protocol_send_player_info(client, server->clients[i].player);
        }
    }
    players_sent[client->fd] = true;
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
    send_existing_players_after_map(server, client);
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

/*static void handle_player_inventory(server_t *server, client_t *client, const char *cmd)
{
    int player_id;
    player_t *player;
    char response[128];

    if (sscanf(cmd, "pin #%d", &player_id) != 1) {
        send_response(client, "sbp\n");
        return;
    }
    player = player_find_by_id(server, player_id);
    if (!player) {
        send_response(client, "sbp\n");
        return;
    }
    snprintf(response, sizeof(response),
        "pin #%d %d %d %d %d %d %d %d %d %d\n",
        player->id, player->x, player->y,
        player->resources[RESOURCE_FOOD],
        player->resources[RESOURCE_LINEMATE],
        player->resources[RESOURCE_DERAUMERE],
        player->resources[RESOURCE_SIBUR],
        player->resources[RESOURCE_MENDIANE],
        player->resources[RESOURCE_PHIRAS],
        player->resources[RESOURCE_THYSTAME]);
    send_response(client, response);
*/

// sgt
static void handle_time_unit_command(server_t *server, client_t *client,
    const char *cmd)
{
    char *response;

    (void)cmd;
    if (server && server->config.freq > 0) {
        asprintf(&response, "sgt %zu\n", server->config.freq);
        send_response(client, response);
        free(response);
    } else {
        send_response(client, "sbp\n");
    }
}

// sst
static void handle_time_unit_modification(server_t *server, client_t *client,
    const char *cmd)
{
    int new_freq;
    char *response;

    if (sscanf(cmd, "sst %d", &new_freq) == 1 && new_freq > 0) {
        server->config.freq = new_freq;
        asprintf(&response, "sgt %zu\n", server->config.freq);
        send_response(client, response);
        free(response);
    } else {
        send_response(client, "sbp\n");
    }
}

static graphic_cmd_handler_t find_graphic_handler(const char *cmd)
{
    size_t i;
    const graphic_cmd_entry_t graphic_commands[] = {
        {"msz", handle_map_size_command},
        {"mct", handle_map_content_command},
        {"tna", handle_team_names_command},
        {"bct ", handle_tile_content_command},
        {"pnw", handle_player_info_command},
        {"ppo", handle_position_update},
        {"pin", handle_player_inventory},
        {"sgt", handle_time_unit_command},
        {"sst", handle_time_unit_modification},
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
