/*
** EPITECH PROJECT, 2025
** /home/vj/coding/zappy/include/server/ai_handler_command/handler4
** File description:
** broadcast header
*/

#ifndef AI_ACTIONS4_H
    #define AI_ACTIONS4_H

    #include <math.h>
    #include <stdio.h>

    #include "server/server.h"

    #ifndef M_PI
        #define M_PI 3.14159265358979323846
    #endif

static inline void helper_send_pbc(server_t *server, client_t *client,
    player_t *sender, const char *msg)
{
    char *buf = NULL;
    client_t *other;

    asprintf(&buf, "pbc #%d %s\n", sender->id, msg);
    for (size_t i = 0; i < server->client_count; ++i) {
        other = &server->clients[i];
        if (other->type == CLIENT_TYPE_GRAPHIC) {
            send_response(other, buf);
        }
    }
    free(buf);
}

static inline int compute_direction(int dx, int dy,
    int width, int height, int orient)
{
    int direction = 0;
    double angle;

    if (dx > width / 2) dx -= width;
    if (dx < -width / 2) dx += width;
    if (dy > height / 2) dy -= height;
    if (dy < -height / 2) dy += height;
    if (dx == 0 && dy == 0) {
        direction = 0;
    } else {
        angle = atan2(-(double)dy, (double)dx) * 180.0 / M_PI;
        if (angle < 0) angle += 360.0;
        if (angle >= 337.5 || angle < 22.5) direction = 3; // E
        else if (angle >= 22.5 && angle < 67.5) direction = 2; // NE
        else if (angle >= 67.5 && angle < 112.5) direction = 1; // N
        else if (angle >= 112.5 && angle < 157.5) direction = 8; // NW
        else if (angle >= 157.5 && angle < 202.5) direction = 7; // W
        else if (angle >= 202.5 && angle < 247.5) direction = 6; // SW
        else if (angle >= 247.5 && angle < 292.5) direction = 5; // S
        else if (angle >= 292.5 && angle < 337.5) direction = 4; // SE
        if (orient < 1 || orient > 4) orient = 1;
        direction = ((direction - 1 + 2 * (orient - 1)) % 8) + 1;
    }
    return direction;
}

static inline void send_broadcast_to_ai(server_t *server,
    client_t *sender_client, player_t *sender, const char *msg)
{
    char *ai_msg = NULL;
    int direction;
    int orient;
    client_t *other;

    for (size_t i = 0; i < server->client_count; ++i) {
        other = &server->clients[i];
        if (other->type != CLIENT_TYPE_AI || other->fd == sender_client->fd
            || !other->player)
            continue;
        orient = other->player->orientation;
        direction = compute_direction((sender->x - other->player->x),
            (sender->y - other->player->y), server->game->map->width,
            server->game->map->height, orient);
        asprintf(&ai_msg, "message %d,%s\n", direction, msg);
        send_response(other, ai_msg);
        free(ai_msg);
    }
}

static inline void ai_action_broadcast(server_t *server, client_t *client,
    char *msg)
{
    player_t *sender;
    int width, height;

    if (!server || !client || !msg || !(sender = client->player)) {
        send_response(client, "ko\n");
        return;
    }
    width = server->game->map->width;
    height = server->game->map->height;
    send_broadcast_to_ai(server, client, sender, msg);
    helper_send_pbc(server, client, sender, msg);
}

#endif
