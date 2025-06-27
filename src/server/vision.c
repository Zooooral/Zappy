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

static char *print_tile(tile_t *tile)
{
    char *str = da_create();
    int first = 1;

    if (!tile)
        return da_create();
    if (tile->player_count) {
        str = da_push(str, "player", 6);
        first = 0;
    }
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < tile->resources[i]; ++j) {
            str = da_push(str, " ", 1);
            str = da_push(str, ressource_string_table[i],
                strlen(ressource_string_table[i]));
            first = 0;
        }
    }
    return str;
}

static void vision_loop_iteration(struct vision_loop_s *s)
{
    if (s->orientation == 1) {
        *s->x = (*s->x + s->offset) % s->map->width;
        *s->y = (*s->y - s->dist + s->map->height) % s->map->height;
    } else if (s->orientation == 2) {
        *s->x = (*s->x + s->dist + s->map->width) % s->map->width;
        *s->y = (*s->y + s->offset) % s->map->height;
        return;
    }
    if (s->orientation == 3) {
        *s->x = (*s->x - s->offset + s->map->width) % s->map->width;
        *s->y = (*s->y + s->dist + s->map->height) % s->map->height;
        return;
    }
    *s->x = (*s->x - s->dist + s->map->width) % s->map->width;
    *s->y = (*s->y - s->offset + s->map->height) % s->map->height;
}

static void add_tile_to_response(char **res, char *tile, int *first)
{
    if (!*first)
        *res = da_push(*res, ", ", 1);
    *res = da_push(*res, tile, DA_LEN(tile));
    da_destroy(tile);
    *first = 0;
}

char *vision_look(client_t *client, map_t *map)
{
    char *res = da_push(da_create(), "[", 1);
    int x;
    int y;
    char *tile = NULL;
    int first = 1;

    for (int dist = 0; dist <= client->player->level; ++dist) {
        for (int offset = -dist; offset <= dist; ++offset) {
            vision_loop_iteration(&(struct vision_loop_s) { &x, &y,
                dist, offset, client->player->orientation, client->player, map
            });
            tile = print_tile(map_get_tile(map, x, y));
            add_tile_to_response(&res, tile, &first);
            first = 0;
        }
    }
    return da_push(res, " ]\n", 4);
}
