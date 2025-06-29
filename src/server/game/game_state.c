/*
** EPITECH PROJECT, 2025
** src/server/game/game_state.c
** File description:
** Game state management with
*/

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "server/resource.h"
#include "server/server.h"
#include "server/lifecycle.h"

double get_current_time(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

static int initialize_players_array(game_state_t *game)
{
    game->player_capacity = 500;
    game->players = calloc(game->player_capacity, sizeof *game->players);
    if (!game->players)
        return -1;
    game->player_count = 0;
    game->next_player_id = 1;
    return 0;
}

static int setup_game_map(game_state_t *game, const server_config_t *config)
{
    game->map = map_create(config->width, config->height);
    if (!game->map)
        return -1;
    return 0;
}

game_state_t *game_state_create(server_t *server,
    const server_config_t *config)
{
    game_state_t *game = calloc(1, sizeof(game_state_t));

    (void)server;
    if (!game)
        return NULL;
    if (setup_game_map(game, config) == -1) {
        free(game);
        return NULL;
    }
    if (initialize_players_array(game) == -1) {
        map_destroy(game->map);
        free(game);
        return NULL;
    }
    game->current_time = get_current_time();
    return game;
}

static void cleanup_players(game_state_t *game)
{
    size_t i;

    if (!game->players)
        return;
    for (i = 0; i < game->player_count; i++)
        player_destroy(game->players[i]);
    free(game->players);
}

void game_state_destroy(game_state_t *game)
{
    if (!game)
        return;
    if (game->map)
        map_destroy(game->map);
    cleanup_players(game);
    free(game);
}

// unsure if this should be one more tick before death
static void player_update(player_t *player, server_t *server)
{
    if (!player || !player->is_alive)
        return;
    player->last_food_inhalation += 1;
    if (player->last_food_inhalation >= FOOD_INHALATION_TIME) {
        player->resources[RESOURCE_FOOD]--;
        player->last_food_inhalation = 0;
        if (player->resources[RESOURCE_FOOD] <= 0)
            player_die(player->client, server);
    }
}

void game_state_update(server_t *server, double delta_time)
{
    player_t *player;

    if (!server || !server->game)
        return;
    server->game->current_time += delta_time;
    for (size_t i = 0; i < server->game->player_count; ++i) {
        player = server->game->players[i];
        if (player == NULL || !player->is_alive) {
            continue;
        }
        player_update(player, server);
    }
}

void add_player_to_game(game_state_t *game, player_t *player)
{
    if (!game || !player)
        return;
    if (game->players != NULL) {
        game->players[game->player_count] = player;
        ++game->player_count;
    }
}
