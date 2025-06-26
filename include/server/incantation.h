/*
** EPITECH PROJECT, 2025
** include/server/incantation.h
** File description:
** Incantation system header for Zappy
*/

#ifndef INCANTATION_H
    #define INCANTATION_H

    #include "server/server.h"
    #include <stdbool.h>

typedef struct {
    server_t *server;
    player_t *initiator;
    tile_t *tile;
    int level;
    const int *reqs;
    player_t *players[8];
    int player_count;
} incantation_ctx_t;

bool incantation_requirements_met(server_t *server, player_t *player);

// Returns 1 if incantation succeeded, 0 if failed
int try_incantation(server_t *server, client_t *client);

#endif // INCANTATION_H
