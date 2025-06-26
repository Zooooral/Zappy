/*
** EPITECH PROJECT, 2025
** src/server/game/player.c
** File description:
** Player management system
*/

#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "server/broadcast.h"
#include "server/payloads.h"
#include "server/server_updates.h"

player_t *player_create(client_t *client, int x, int y, const char *team_name)
{
    player_t *player = calloc(1, sizeof *player);

    if (!player)
        return NULL;
    player->id = client->fd;
    player->x = x;
    player->y = y;
    player->orientation = 1;
    player->level = 1;
    player->client = client;
    player->team_name = strdup(team_name);
    if (!player->team_name) {
        free(player);
        return NULL;
    }
    player->resources[RESOURCE_FOOD] = 10;
    player->last_action_time = get_current_time();
    player->is_alive = true;
    return player;
}

void player_destroy(player_t *player)
{
    if (!player)
        return;
    if (player->team_name)
        free(player->team_name);
    free(player);
}

static void shift_players_in_tile(tile_t *tile, size_t removed_index)
{
    size_t j;

    for (j = removed_index; j < tile->player_count - 1; j++)
        tile->players[j] = tile->players[j + 1];
}

static void remove_player_from_tile(player_t *player, map_t *map)
{
    tile_t *tile = map_get_tile(map, player->x, player->y);
    size_t i;

    if (!tile)
        return;
    for (i = 0; i < tile->player_count; i++) {
        if (tile->players[i] == player) {
            shift_players_in_tile(tile, i);
            tile->player_count--;
            break;
        }
    }
}

static int add_player_to_tile(player_t *player, map_t *map)
{
    tile_t *tile = map_get_tile(map, player->x, player->y);

    if (!tile)
        return -1;
    if (tile->player_count >= tile->player_capacity)
        return -1;
    tile->players[tile->player_count] = player;
    tile->player_count++;
    return 0;
}

void player_set_position(server_t *server, player_t *player, int x, int y)
{
    if (!player || !server)
        return;
    remove_player_from_tile(player, server->game->map);
    player->x = (x + server->game->map->width) % server->game->map->width;
    player->y = (y + server->game->map->height) % server->game->map->height;
    add_player_to_tile(player, server->game->map);
    broadcast_message_to_guis(server, player, gui_payload_position_update);
}

player_t *player_find_by_id(server_t *server, int id)
{
    if (!server || !server->game || !server->game->players)
        return NULL;
    for (size_t i = 0; i < server->game->player_count; ++i) {
        if (server->game->players[i]->id == id)
            return server->game->players[i];
    }
    return NULL;
}
