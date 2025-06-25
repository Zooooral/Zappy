/*
** EPITECH PROJECT, 2025
** src/server/incantation.c
** File description:
** Incantation system implementation for Zappy
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "server/incantation.h"
#include "server/server.h"

static const int incantation_requirements[8][7] = {
    {1, 0, 0, 0, 0, 0, 0}, // Level 1 (unused)
    {1, 1, 0, 0, 0, 0, 0}, // Level 2
    {2, 1, 1, 1, 0, 0, 0}, // Level 3
    {2, 2, 0, 1, 0, 2, 0}, // Level 4
    {4, 1, 1, 2, 0, 1, 0}, // Level 5
    {4, 1, 2, 1, 3, 0, 0}, // Level 6
    {6, 1, 2, 3, 0, 1, 0}, // Level 7
    {6, 2, 2, 2, 2, 2, 1}, // Level 8
};

static tile_t *get_player_tile(server_t *server, player_t *p) {
    return map_get_tile(server->game->map, p->x, p->y);
}

static int get_player_level(player_t *p) {
    return p ? p->level : 1;
}

static void gather_players_on_tile(server_t *server, tile_t *tile, int level, player_t **out, int *count) {
    int n = 0;
    for (size_t i = 0; i < server->game->player_count; ++i) {
        player_t *p = server->game->players[i];
        if (p && p->x == tile->players[0]->x && p->y == tile->players[0]->y && p->level == level && p->is_alive)
            out[n++] = p;
    }
    *count = n;
}

static bool has_required_players(server_t *server, tile_t *tile, int level, int required, player_t **out, int *count) {
    gather_players_on_tile(server, tile, level, out, count);
    return *count >= required;
}

static bool has_required_resources(tile_t *tile, const int *reqs) {
    for (int i = 1; i < 7; ++i) {
        if (tile->resources[i] < reqs[i])
            return false;
    }
    return true;
}

static void remove_resources(tile_t *tile, const int *reqs) {
    for (int i = 1; i < 7; ++i)
        tile->resources[i] -= reqs[i];
}

static void notify_gui_pic(server_t *server, tile_t *tile, int level, player_t **players, int count) {
    char *buf = NULL;
    int len = 0;
    asprintf(&buf, "pic %d %d %d", tile->players[0]->x, tile->players[0]->y, level);
    for (int i = 0; i < count; ++i) {
        char *tmp = NULL;
        asprintf(&tmp, "%s #%d", buf, players[i]->id);
        free(buf);
        buf = tmp;
    }
    char *tmp2 = NULL;
    asprintf(&tmp2, "%s\n", buf);
    free(buf);
    buf = tmp2;
    for (size_t i = 0; i < server->client_count; ++i)
        if (server->clients[i].type == CLIENT_TYPE_GRAPHIC)
            send_response(&server->clients[i], buf);
    free(buf);
}

static void notify_gui_pie(server_t *server, tile_t *tile, int result) {
    char *buf = NULL;
    asprintf(&buf, "pie %d %d %d\n", tile->players[0]->x, tile->players[0]->y, result);
    for (size_t i = 0; i < server->client_count; ++i)
        if (server->clients[i].type == CLIENT_TYPE_GRAPHIC)
            send_response(&server->clients[i], buf);
    free(buf);
}

static void notify_gui_level_up(server_t *server, player_t *p) {
    char *buf = NULL;
    asprintf(&buf, "plv #%d %d\n", p->id, p->level);
    for (size_t i = 0; i < server->client_count; ++i)
        if (server->clients[i].type == CLIENT_TYPE_GRAPHIC)
            send_response(&server->clients[i], buf);
    free(buf);
}

static void notify_gui_inventory(server_t *server, player_t *p) {
    char *buf = NULL;
    asprintf(&buf, "pin #%d %d %d %d %d %d %d %d %d %d\n", p->id, p->x, p->y,
        p->resources[0], p->resources[1], p->resources[2], p->resources[3],
        p->resources[4], p->resources[5], p->resources[6]);
    for (size_t i = 0; i < server->client_count; ++i)
        if (server->clients[i].type == CLIENT_TYPE_GRAPHIC)
            send_response(&server->clients[i], buf);
    free(buf);
}

// Returns 1 if incantation succeeded, 0 if failed
int try_incantation(server_t *server, client_t *client) {
    if (!server || !client || !client->player)
        return 0;
    player_t *p = client->player;
    tile_t *tile = get_player_tile(server, p);
    int level = get_player_level(p);
    if (level < 1 || level > 7 || !tile)
        return 0;
    const int *reqs = incantation_requirements[level];
    player_t *players[8] = {0};
    int player_count = 0;
    if (!has_required_players(server, tile, level, reqs[0], players, &player_count)) {
        notify_gui_pic(server, tile, level, players, player_count);
        notify_gui_pie(server, tile, 0);
        return 0;
    }
    if (!has_required_resources(tile, reqs)) {
        notify_gui_pic(server, tile, level, players, player_count);
        notify_gui_pie(server, tile, 0);
        return 0;
    }
    remove_resources(tile, reqs);
    for (int i = 0; i < player_count; ++i) {
        players[i]->level++;
        notify_gui_level_up(server, players[i]);
        notify_gui_inventory(server, players[i]);
    }
    notify_gui_pic(server, tile, level, players, player_count);
    notify_gui_pie(server, tile, 1);
    return 1;
}
