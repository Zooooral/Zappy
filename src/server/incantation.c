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
#include "server/payloads.h"
#include "server/broadcast.h"

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

tile_t *get_player_tile(server_t *server, player_t *p) {
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

static void remove_resources(incantation_ctx_t *ctx, const int *reqs) {
    for (int i = 1; i < 7; ++i)
        ctx->tile->resources[i] -= reqs[i];
    broadcast_tile_to_guis(ctx->server, ctx->tile->x, ctx->tile->y);
}

bool incantation_requirements_met(server_t *server, player_t *player) {
    if (!server || !player)
        return false;
    tile_t *tile = get_player_tile(server, player);
    int level = get_player_level(player);
    if (level < 1 || level > 7 || !tile)
        return false;
    const int *reqs = incantation_requirements[level];
    player_t *players[8] = {0};
    int player_count = 0;
    if (!has_required_players(server, tile, level, reqs[0], players, &player_count))
        return false;
    if (!has_required_resources(tile, reqs))
        return false;
    return true;
}

static int do_incantation(incantation_ctx_t *ctx) {
    remove_resources(ctx->server, ctx->reqs);
    ctx->initiator->level++;
    broadcast_message_to_guis(ctx->server, ctx->initiator, gui_payload_plv);
    broadcast_message_to_guis(ctx->server, ctx->initiator, gui_payload_pin);
    broadcast_message_to_guis(ctx->server, ctx->initiator, gui_payload_pie_success);
    return 1;
}

int try_incantation(server_t *server, client_t *client) {
    if (!server || !client || !client->player)
        return 0;
    player_t *p = client->player;
    tile_t *tile = get_player_tile(server, p);
    int level = get_player_level(p);
    if (level < 1 || level > 7 || !tile)
        return 0;
    const int *reqs = incantation_requirements[level];
    incantation_ctx_t ctx = { server, p, tile, level, reqs, {p}, reqs[0] };
    if (!incantation_requirements_met(server, p)) {
        broadcast_message_to_guis(server, p, gui_payload_pie_failed);
        return 0;
    }
    return do_incantation(&ctx);
}
