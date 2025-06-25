/*
** EPITECH PROJECT, 2025
** src/server/vision.c
** File description:
** Vision system implementation for Zappy
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "server/vision.h"
#include "server/dynamic_array.h"
#include "server/game.h"
#include "server/server.h"

static char *print_tile(tile_t *tile) {
    char *str = da_create();
    if (!tile)
        return da_create();
    int first = 1;
    if (tile->player_count) {
        str = da_push(str, "player", 6);
        first = 0;
    }
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < tile->resources[i]; ++j) {
            str = da_push(str, " ", 1);
            str = da_push(str, ressource_string_table[i], strlen(ressource_string_table[i]));
            first = 0;
        }
    }
    return str;
}

char *vision_look(client_t *client, map_t *map)
{
    char *res = da_create();
    int x, y;
    char *tile = NULL;
    int first = 1;
    res = da_push(res, "[", 1);
    for (int dist = 0; dist <= client->player->level; ++dist) {
        for (int offset = -dist; offset <= dist; ++offset) {
            if (client->player->orientation == 1) {
                x = (client->player->x + offset + map->width) % map->width;
                y = (client->player->y - dist + map->height) % map->height;
            } else if (client->player->orientation == 2) {
                x = (client->player->x + dist + map->width) % map->width;
                y = (client->player->y + offset + map->height) % map->height;
            } else if (client->player->orientation == 3) {
                x = (client->player->x - offset + map->width) % map->width;
                y = (client->player->y + dist + map->height) % map->height;
            } else {
                x = (client->player->x - dist + map->width) % map->width;
                y = (client->player->y - offset + map->height) % map->height;
            }
            if (!first)
                res = da_push(res, ", ", 1);
            tile = print_tile(map_get_tile(map, x, y));
            res = da_push(res, tile, DA_LEN(tile));
            da_destroy(tile);
            first = 0;
        }
    }
    DA_PUSH(res, " ]\n");
    return res;
}