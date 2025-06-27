/*
** EPITECH PROJECT, 2025
** /home/vj/coding/zappy/src/server/protocol/protocol_graphic_garbage
** File description:
** you know what this is
*/


#ifndef PROTOCOL_GRAPHIC_GARBAGE_H
    #define PROTOCOL_GRAPHIC_GARBAGE_H

static inline void handle_position_update(server_t *server, client_t *client,
    const char *cmd)
{
    int player_id;
    player_t *player;

    sscanf(cmd, "ppo #%d", &player_id);
    player = player_find_by_id(server, player_id);
    if (player) {
        send_position_update(client, player);
    }
}

static inline void handle_player_inventory(server_t *server, client_t *client,
    const char *cmd)
{
    int player_id;
    player_t *player;
    char *response;

    if (sscanf(cmd, "pin #%d", &player_id) != 1) {
        send_response(client, "sbp\n");
        return;
    }
    player = player_find_by_id(server, player_id);
    if (!player) {
        send_response(client, "sbp\n");
        return;
    }
    response = gui_payload_pin(client, player);
    if (!response) {
        send_response(client, "sbp\n");
        return;
    }
    send_response(client, response);
}

// sgt
static inline void handle_time_unit_command(server_t *server, client_t *client,
    const char *cmd)
{
    char *response;

    (void)cmd;
    if (server && server->config.freq > 0) {
        asprintf(&response, "sgt %zu\n", server->config.freq);
        send_response(client, response);
        free(response);
    } else {
        send_response(client, "sbp\n");
    }
}

// sst
static inline void handle_time_unit_modification(server_t *server,
    client_t *client, const char *cmd)
{
    int new_freq;
    char *response;

    if (sscanf(cmd, "sst %d", &new_freq) == 1 && new_freq > 0) {
        server->config.freq = new_freq;
        asprintf(&response, "sgt %zu\n", server->config.freq);
        send_response(client, response);
        free(response);
    } else {
        send_response(client, "sbp\n");
    }
}
#endif // PROTOCOL_GRAPHIC_GARBAGE_H
