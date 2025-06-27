/*
** EPITECH PROJECT, 2025
** include/server/broadcast.h
** File description:
** Broadcast system header for Zappy
*/

#ifndef BROADCAST_H
    #define BROADCAST_H

    #include "server/server.h"

void broadcast_message_to_guis(server_t *server, player_t *player,
    char *(*function)(client_t *, const player_t *));
void broadcast_tile_to_guis(server_t *server, int x, int y);
void broadcast_player_resource_update(server_t *server, player_t *player,
    int resource_id, char *(*function)(const player_t *, int));

#endif // BROADCAST_H
