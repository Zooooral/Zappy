/*
** EPITECH PROJECT, 2025
** include/server/payloads.h
** File description:
** Network event handling functions
*/

#ifndef NETWORK_PAYLOADS_H_
    #define NETWORK_PAYLOADS_H_

    #include "server.h"

/* Player-related payloads */
char *gui_payload_pnw(client_t *, const player_t *player);
char *gui_payload_ppo(client_t *, const player_t *player);
char *gui_payload_plv(client_t *, const player_t *player);
char *gui_payload_pdi(client_t *, const player_t *player);

/* Incantation payloads */
char *gui_payload_pic(tile_t *tile, int level, player_t **players, int count);
char *gui_payload_pie_success(client_t *, const player_t *player);
char *gui_payload_pie_failed(client_t *, const player_t *player);

/* Tile payloads */
char *gui_payload_tile(server_t *server, int x, int y);

/* Resource payloads */
char *gui_payload_pgt(const player_t *player, int resource_id);
char *gui_payload_pdr(const player_t *player, int resource_id);
char *gui_payload_pin(client_t *, const player_t *player);

/* Egg payloads */
char *gui_payload_egg_laid(int egg_id, const player_t *player);
char *gui_payload_egg_hatched(int egg_id);
char *gui_payload_egg_died(int egg_id);

/* Game state payloads */
char *gui_payload_game_end(int winning_team_id);
char *gui_payload_time_unit_get(int time_unit);
char *gui_payload_time_unit_set(int time_unit);

#endif /* !NETWORK_PAYLOADS_H_ */
