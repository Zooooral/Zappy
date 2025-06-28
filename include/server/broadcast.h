/*
** EPITECH PROJECT, 2025
** include/server/broadcast.h
** File description:
** Broadcast system header for Zappy
*/

#ifndef BROADCAST_H
    #define BROADCAST_H

    #include "server/server.h"

void broadcast_string_message_to_guis(server_t *server, const char *message);
void broadcast_message_to_guis(server_t *server, player_t *player,
    char *(*function)(client_t *, const player_t *));
void broadcast_tile_to_guis(server_t *server, int x, int y);
void broadcast_player_resource_update(server_t *server, player_t *player,
    int resource_id, char *(*function)(const player_t *, int));

/* Player-related broadcasts */
void broadcast_player_death(server_t *server, player_t *player);

/* Game state broadcasts */
void broadcast_game_end(server_t *server, int winning_team_id);
void broadcast_time_unit(server_t *server);
void broadcast_time_unit_modified(server_t *server, int time_unit);

/* Egg-related broadcasts */
void broadcast_egg_laid(server_t *server, int egg_id, player_t *player);
void broadcast_egg_hatched(server_t *server, int egg_id);
void broadcast_egg_died(server_t *server, int egg_id);

#endif // BROADCAST_H
