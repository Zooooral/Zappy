/*
** EPITECH PROJECT, 2025
** include/server/payloads.h
** File description:
** Network event handling functions
*/

#ifndef NETWORK_PAYLOADS_H_
    #define NETWORK_PAYLOADS_H_

    #include "server.h"

const char *gui_payload_pnw(client_t *, const player_t *player);
const char *gui_payload_ppo(client_t *, const player_t *player);
const char *gui_payload_plv(client_t *, const player_t *player);
const char *gui_payload_pdi(client_t *, const player_t *player);

const char *gui_payload_elevation_start(client_t *, const player_t *player);
const char *gui_payload_elevation_end(client_t *, const player_t *player);

char *gui_payload_tile(server_t *server, int x, int y);

char *gui_payload_pgt(const player_t *player, int resource_id);
char *gui_payload_pdr(const player_t *player, int resource_id);
const char *gui_payload_pdr(client_t *, const player_t *player);

#endif /* !NETWORK_PAYLOADS_H_ */
