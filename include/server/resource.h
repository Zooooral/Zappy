/*
** EPITECH PROJECT, 2025
** include/server/resource.h
** File description:
** Resource management header for Zappy
*/

#ifndef RESOURCE_H
    #define RESOURCE_H

    #include "server/server.h"

int take_resource(client_t *client, map_t *map, int resource_id);
int set_resource(client_t *client, map_t *map, int resource_id);
void respawn_resources(server_t *server);

#endif // RESOURCE_H
