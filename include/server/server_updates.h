/*
** EPITECH PROJECT, 2025
** include/server/server_updates.h
** File description:
** Server update sending functionality
*/

#ifndef SERVER_UPDATES_H_
    #define SERVER_UPDATES_H_

    #include "server.h"

typedef struct {
    bool position_changed;
    bool level_changed;
    bool elevation_changed;
} update_flags_t;

void send_position_update(client_t *client, const player_t *player);

#endif /* !SERVER_UPDATES_H_ */
