/*
** EPITECH PROJECT, 2025
** include/server/protocol_graphic.h
** File description:
** Protocol handling for graphic clients
*/

#ifndef PROTOCOL_GRAPHIC_H_
    #define PROTOCOL_GRAPHIC_H_

    #include "server.h"

typedef void (*graphic_cmd_handler_t)(server_t *server, client_t *client,
    const char *cmd);

typedef struct {
    const char *cmd;
    graphic_cmd_handler_t handler;
} graphic_cmd_entry_t;

void protocol_handle_graphic_command(server_t *server, client_t *client,
    const char *cmd);
void protocol_send_map_size(server_t *server, client_t *client);
void protocol_send_tile_content(server_t *server, client_t *client,
    int x, int y);
void protocol_send_player_info(client_t *client, const player_t *player);

#endif /* !PROTOCOL_GRAPHIC_H_ */
