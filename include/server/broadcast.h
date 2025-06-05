/*
** EPITECH PROJECT, 2025
** include/server/broadcast.h
** File description:
** Broadcast system header for Zappy
*/

#ifndef BROADCAST_H
    #define BROADCAST_H

    #include "server/server.h"

void broadcast_message(client_t *sender, const char *msg);
void broadcast_message_to_guis(server_t *server, client_t *sender,
    void (*function)(client_t *, const player_t *));

#endif // BROADCAST_H
