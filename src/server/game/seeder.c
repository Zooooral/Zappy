/*
** EPITECH PROJECT, 2025
** src/server/game/seeder.c
** File description:
** Seeder functionality for GUI testing
*/

#include <stdlib.h>
#include <string.h>

#include "server/server.h"

static const int circle_moves[][2] = {
    {0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}
};

static const int circle_moves_count = 5;

seeder_state_t *seeder_create(map_t *map)
{
    seeder_state_t *seeder = malloc(sizeof(seeder_state_t));

    if (!seeder)
        return NULL;
    memset(seeder, 0, sizeof(seeder_state_t));
    seeder->player = player_create(NULL, 0, 0, "Seeders");
    if (!seeder->player) {
        free(seeder);
        return NULL;
    }
    player_set_position(seeder->player, map, 0, 0);
    seeder->last_move_time = get_current_time();
    seeder->last_elevation_time = get_current_time();
    seeder->move_step = 0;
    seeder->elevation_active = false;
    printf("[SEED] Created seeder player at (0,0)\n");
    return seeder;
}

void seeder_destroy(seeder_state_t *seeder)
{
    if (!seeder)
        return;
    if (seeder->player)
        player_destroy(seeder->player);
    free(seeder);
}

static void seeder_move_player(seeder_state_t *seeder, map_t *map)
{
    int target_x = circle_moves[seeder->move_step][0];
    int target_y = circle_moves[seeder->move_step][1];

    player_set_position(seeder->player, map, target_x, target_y);
    seeder->move_step = (seeder->move_step + 1) % circle_moves_count;
    printf("[SEED] Moved player to (%d,%d)\n", target_x, target_y);
}

static void start_elevation_cycle(seeder_state_t *seeder, double current_time)
{
    seeder->elevation_active = true;
    seeder->player->is_elevating = true;
    seeder->player->elevation_start_time = current_time;
    seeder->last_elevation_time = current_time;
    printf("[SEED] Started elevation cycle\n");
}

static void complete_elevation_cycle(seeder_state_t *seeder)
{
    seeder->elevation_active = false;
    seeder->player->is_elevating = false;
    if (seeder->player->level < 8)
        seeder->player->level++;
    printf("[SEED] Completed elevation cycle, level: %d\n",
        seeder->player->level);
}

static void handle_active_elevation(seeder_state_t *seeder,
    double current_time)
{
    double elevation_duration = current_time -
        seeder->player->elevation_start_time;

    if (elevation_duration >= SEEDER_ELEVATION_DURATION)
        complete_elevation_cycle(seeder);
}

static void seeder_handle_elevation(seeder_state_t *seeder,
    double current_time)
{
    double time_since_last = current_time - seeder->last_elevation_time;

    if (!seeder->elevation_active &&
        time_since_last >= SEEDER_ELEVATION_INTERVAL) {
        start_elevation_cycle(seeder, current_time);
    } else if (seeder->elevation_active) {
        handle_active_elevation(seeder, current_time);
    }
}

void seeder_update(seeder_state_t *seeder, map_t *map, double current_time)
{
    double time_since_move;

    if (!seeder || !map)
        return;
    time_since_move = current_time - seeder->last_move_time;
    if (time_since_move >= SEEDER_MOVE_INTERVAL) {
        seeder_move_player(seeder, map);
        seeder->last_move_time = current_time;
    }
    seeder_handle_elevation(seeder, current_time);
}
