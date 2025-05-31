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

#endif // BROADCAST_H
