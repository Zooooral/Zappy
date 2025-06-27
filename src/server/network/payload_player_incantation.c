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

static int append_players_to_buf(char **buf, player_t **players, int count)
{
    char *tmp = NULL;
    int ret;

    for (int i = 0; i < count; ++i) {
        tmp = NULL;
        ret = asprintf(&tmp, "%s #%d", *buf, players[i]->id);
        if (ret == -1) {
            free(*buf);
            return -1;
        }
        free(*buf);
        *buf = tmp;
    }
    return 0;
}

char *gui_payload_pic(tile_t *tile, int level, player_t **players, int count)
{
    char *buf = NULL;
    char *tmp2 = NULL;
    int ret;

    ret = asprintf(&buf, "pic %d %d %d",
        tile->players[0]->x,
        tile->players[0]->y,
        level);
    if (ret == -1)
        return NULL;
    if (append_players_to_buf(&buf, players, count) == -1)
        return NULL;
    ret = asprintf(&tmp2, "%s\n", buf);
    if (ret == -1) {
        free(buf);
        return NULL;
    }
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
