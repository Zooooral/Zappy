/*
** EPITECH PROJECT, 2025
** /home/vj/coding/zappy/include/server/game
** File description:
** game logic
*/

#ifndef GAME_H_
    #define GAME_H_

    #include <stdint.h>
    #include <assert.h>
    #include "server/server.h"

typedef enum {
    RESSOURCE_FOOD,
    RESSOURCE_LINEMATE,
    RESSOURCE_DERAUMERE,
    RESSOURCE_SIBUR,
    RESSOURCE_MENDIANE,
    RESSOURCE_PHIRAS,
    RESSOURCE_THYSTAME,
    RESSOURCE_COUNT,
} ressource_t;

static const char *ressource_string_table[] = {
    [RESSOURCE_FOOD] = "food",
    [RESSOURCE_LINEMATE] = "linemate",
    [RESSOURCE_DERAUMERE] = "deraumere",
    [RESSOURCE_SIBUR] = "sibur",
    [RESSOURCE_MENDIANE] = "mendiane",
    [RESSOURCE_PHIRAS] = "phiras",
    [RESSOURCE_THYSTAME] = "thystame",
};

static const uint32_t ressource_density_percent[RESSOURCE_COUNT] = {
    [RESSOURCE_FOOD] = 50,
    [RESSOURCE_LINEMATE] = 30,
    [RESSOURCE_DERAUMERE] = 15,
    [RESSOURCE_SIBUR] = 10,
    [RESSOURCE_MENDIANE] = 10,
    [RESSOURCE_PHIRAS] = 8,
    [RESSOURCE_THYSTAME] = 5,
};

static inline uint32_t ressource_quantity(map_t *map,
    ressource_t ressource)
{
    assert(ressource < RESSOURCE_COUNT);
    return (map->width * map->height *
        ressource_density_percent[ressource]) / 100;
}

#endif /* !GAME_H_ */
