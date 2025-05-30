/*
** EPITECH PROJECT, 2025
** include/server/server_broadcast.h
** File description:
** Server broadcasting functionality
*/

#ifndef SERVER_BROADCAST_H_
    #define SERVER_BROADCAST_H_

    #include "server.h"

typedef struct {
    int x;
    int y;
    int level;
    bool elevating;
} player_state_t;

void broadcast_seeder_updates(server_t *server);

#endif /* !SERVER_BROADCAST_H_ */
