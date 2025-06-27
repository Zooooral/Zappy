/*
** EPITECH PROJECT, 2025
** src/server/network/payloads.c
** File description:
** Server update sending functionality
*/

#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#include "server/server.h"
#include "server/server_updates.h"

char *gui_payload_pic(tile_t *tile, int level, player_t **players, int count)
{
    char *buf = NULL;
    char *tmp2 = NULL;

    asprintf(&buf, "pic %d %d %d", tile->players[0]->x, tile->players[0]->y, level);
    for (int i = 0; i < count; ++i) {
        char *tmp = NULL;
        asprintf(&tmp, "%s #%d", buf, players[i]->id);
        free(buf);
        buf = tmp;
    }
    asprintf(&tmp2, "%s\n", buf);
    free(buf);
    buf = tmp2;
    return buf;
}

char *gui_payload_pie_success(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pie %d %d 1\n",
        player->x, player->y);
    return strdup(response);
}

char *gui_payload_pie_failed(client_t *, const player_t *player)
{
    char response[131];

    snprintf(response, sizeof(response), "pie %d %d 0\n",
        player->x, player->y);
    return strdup(response);
}
