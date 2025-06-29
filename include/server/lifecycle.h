/*
** EPITECH PROJECT, 2025
** include/server/lifecycle.h
** File description:
** Player lifecycle management header for Zappy
*/

#ifndef LIFECYCLE_H
    #define LIFECYCLE_H

    #include "server/server.h"

void consume_food(client_t *client);
void player_die(client_t *client, server_t *server);
void fork_player(client_t *client, map_t *map);

#endif // LIFECYCLE_H
