/*
** EPITECH PROJECT, 2025
** include/server/resource.h
** File description:
** Resource management header for Zappy
*/

#ifndef RESOURCE_H
    #define RESOURCE_H

    #include <string.h>

    #include "server/server.h"
    #include "server/game.h"

int take_resource(client_t *client, map_t *map, int resource_id);
void respawn_resources(server_t *server);

static inline int ressource_string_to_id(const char *resource)
{
    if (!resource)
        return -1;
    for (int i = 0; i < RESOURCE_COUNT; ++i) {
        if (strcmp(resource, ressource_string_table[i]) == 0) {
            return i;
        }
    }
    return -1;
}

#endif // RESOURCE_H
