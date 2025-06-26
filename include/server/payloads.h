/*
** EPITECH PROJECT, 2025
** include/server/payloads.h
** File description:
** Network event handling functions
*/

#ifndef NETWORK_PAYLOADS_H_
    #define NETWORK_PAYLOADS_H_

    #include "server.h"

const char *gui_payload_new_player(client_t *, const player_t *player);
const char *gui_payload_position_update(client_t *, const player_t *player);
const char *gui_payload_level_update(client_t *, const player_t *player);
const char *gui_payload_player_death(client_t *, const player_t *player);

const char *gui_payload_elevation_start(client_t *, const player_t *player);
const char *gui_payload_elevation_end(client_t *, const player_t *player);

char *gui_payload_tile(server_t *server, int x, int y);

char *gui_payload_resource_collected(const player_t *player, int resource_id);
char *gui_payload_resource_dropped(const player_t *player, int resource_id);
const char *gui_payload_inventory(client_t *, const player_t *player);

#endif /* !NETWORK_PAYLOADS_H_ */
