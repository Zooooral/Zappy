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
        return ""; // might be a bad idea
    if (tile->player_count)
        str = da_push(str, "player", 6);
    for (int i = 0; i < 7; ++i) {
        if (tile->resources[i] != 0) {
            if (DA_LEN(str) != 0) {
                DA_PUSH(str, *" ");
            }
            str = da_push(str, ressource_string_table[i], strlen(ressource_string_table[i]));
        }
    }
    DA_PUSH(str, *"");
    return tile;
}

char *vision_look(client_t *client, map_t *map)
{
    char *res = da_create();
    size_t x;
    size_t y;
    char *tile = NULL;

    DA_PUSH(res, *"[");
    res = da_push(res, client->player->team_name, strlen(client->player->team_name));
    for (size_t i = 0; i < client->player->level; ++i) {
        for (size_t j = -i; j <= i; ++j) {
            if (client->player->orientation % 2) {
                x = client->player->x + (i * (client->player->orientation == 1 ? 1 : -1));
                y = client->player->y + j;
            } else {
                x = client->player->x + j;
                y = client->player->y + (i * (client->player->orientation ? 1 : -1));
            }
            if (tile != NULL)
                DA_PUSH(res, *",");
            tile = print_tile(map_get_tile(map, x, y));
            res = da_push(res, tile, strlen(tile));
            da_destroy(tile);
        }
    }
    DA_PUSH(res, "]");
    return res;
}
