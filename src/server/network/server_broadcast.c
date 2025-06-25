/*
** EPITECH PROJECT, 2025
** src/server/network/server_broadcast.c
** File description:
** Server broadcasting functionality
*/

#include "server/server.h"
#include "server/server_broadcast.h"
#include "server/server_updates.h"

static bool player_state_changed(const player_t *player,
    const player_state_t *last_state)
{
    bool position_changed = (player->x != last_state->x ||
        player->y != last_state->y);
    bool level_changed = (player->level != last_state->level);
    bool elevation_changed = (player->is_elevating != last_state->elevating);

    return position_changed || level_changed || elevation_changed;
}

static update_flags_t get_update_flags(const player_t *player,
    const player_state_t *last_state)
{
    update_flags_t flags;

    flags.position_changed = (player->x != last_state->x ||
        player->y != last_state->y);
    flags.level_changed = (player->level != last_state->level);
    flags.elevation_changed = (player->is_elevating != last_state->elevating);
    return flags;
}

static void send_updates_to_client(client_t *client, const player_t *player,
    const update_flags_t *flags)
{
    if (flags->position_changed)
        send_position_update(client, player);
    if (flags->level_changed)
        send_level_update(client, player);
    if (flags->elevation_changed && player->is_elevating)
        send_elevation_start(client, player);
    else if (flags->elevation_changed && !player->is_elevating)
        send_elevation_end(client, player);
}

static void broadcast_to_graphic_clients(server_t *server,
    const player_t *player, const update_flags_t *flags)
{
    client_t *client;
    size_t i;

    for (i = 0; i < server->client_count; i++) {
        client = &server->clients[i];
        if (client->type == CLIENT_TYPE_GRAPHIC &&
            client->is_authenticated) {
            send_updates_to_client(client, player, flags);
        }
    }
}

static void update_last_state(player_state_t *last_state,
    const player_t *player)
{
    last_state->x = player->x;
    last_state->y = player->y;
    last_state->level = player->level;
    last_state->elevating = player->is_elevating;
}
