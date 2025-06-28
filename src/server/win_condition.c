/*
** EPITECH PROJECT, 2025
** src/server/win_condition.c
** File description:
** Game win condition implementation for Zappy
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "server/broadcast.h"

static int count_players_at_level(server_t *server, const char *team_name, int level)
{
    int count = 0;
    for (size_t i = 0; i < server->game->player_count; i++) {
        player_t *player = server->game->players[i];
        if (player && player->team_name && 
            player->is_alive &&
            player->level >= level && 
            strcmp(player->team_name, team_name) == 0) {
            count++;
        }
    }
    return count;
}

// Check if a team has won (6 players at level 8)
int check_win_condition(server_t *server)
{
    char *team_name;
    int level8_count;

    if (!server || !server->game)
        return -1;
    for (size_t t = 0; t < server->config.team_count; t++) {
        team_name = server->config.team_names[t];
        level8_count = count_players_at_level(server, team_name, 8);
        if (level8_count >= 6) {
            return t;
        }
    }
    return -1;
}

void handle_game_win(server_t *server, int winning_team_id)
{
    if (!server || winning_team_id < 0 || 
        winning_team_id >= (int)server->config.team_count)
        return;
    printf("Game won by team %s (ID: %d)\n", 
           server->config.team_names[winning_team_id], winning_team_id);
    broadcast_game_end(server, winning_team_id);
    printf("Game has ended. Server will stop\n");
    server->is_running = false;
}
