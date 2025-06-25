/*
** EPITECH PROJECT, 2025
** include/server/incantation.h
** File description:
** Incantation system header for Zappy
*/

#ifndef INCANTATION_H
    #define INCANTATION_H

    #include "server/server.h"

int incantation_start(client_t *client, map_t *map);
int incantation_end(client_t *client, map_t *map);
int try_incantation(server_t *server, client_t *client);

#endif // INCANTATION_H
