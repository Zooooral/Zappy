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
    int ret;

    (void)client;
    ret = asprintf(&buf, "pbc #%d %s\n", sender->id, msg);
    if (ret < 0 || !buf)
        return;
    for (size_t i = 0; i < server->client_count; ++i) {
        other = &server->clients[i];
        if (other->type == CLIENT_TYPE_GRAPHIC) {
            send_response(other, buf);
        }
    }
    free(buf);
}

static inline int compute_direction_from_angle(double angle, int orient)
{
    int direction = 0;

    if (angle >= 337.5 || angle < 22.5)
        direction = 3;
    if (angle >= 22.5 && angle < 67.5)
        direction = 2;
    if (angle >= 67.5 && angle < 112.5)
        direction = 1;
    if (angle >= 112.5 && angle < 157.5)
        direction = 8;
    if (angle >= 157.5 && angle < 202.5)
        direction = 7;
    if (angle >= 202.5 && angle < 247.5)
        direction = 6;
    if (angle >= 247.5 && angle < 292.5)
        direction = 5;
    if (angle >= 292.5 && angle < 337.5)
        direction = 4;
    orient = (orient < 1 || orient > 4) ? 1 : orient;
    return ((direction - 1 + 2 * (orient - 1)) % 8) + 1;
}

static inline int compute_direction(int *xy,
    int width, int height, int orient)
{
    int direction = 0;
    double angle;

    if (*xy > width / 2)
        *xy -= width;
    if (*xy < (-width / 2))
        *xy += width;
    if (*(xy + 1) > height / 2)
        *(xy + 1) -= height;
    if (*(xy + 1) < (-height / 2))
        *(xy + 1) += height;
    if (*xy == 0 && *(xy + 1) == 0) {
        direction = 0;
    } else {
        angle = atan2(-(double)*(xy + 1), (double)*xy) * 180.0 / M_PI;
        angle += angle < 0 ? 360.0 : 0.0;
        direction = compute_direction_from_angle(angle, orient);
    }
    return direction;
}

static inline void send_broadcast_to_ai(server_t *server,
    client_t *sender_client, player_t *sender, const char *msg)
{
    char *ai_msg = NULL;
    int orient;
    client_t *other;
    int ret;

    for (size_t i = 0; i < server->client_count; ++i) {
        other = &server->clients[i];
        if (other->type != CLIENT_TYPE_AI || other->fd == sender_client->fd
            || !other->player)
            continue;
        orient = other->player->orientation;
        ret = asprintf(&ai_msg, "message %d, %s\n", compute_direction((int[2])
            {(sender->x - other->player->x),
            (sender->y - other->player->y)}, server->game->map->width,
            server->game->map->height, orient), msg);
        if (ret < 0 || !ai_msg)
            continue;
        send_response(other, ai_msg);
        free(ai_msg);
    }
}

static inline void ai_action_broadcast(server_t *server, client_t *client,
    char *msg)
{
    player_t *sender = client ? client->player : NULL;

    if (!server || !client || !msg || !sender) {
        send_response(client, "ko\n");
        return;
    }
    send_broadcast_to_ai(server, client, sender, msg);
    helper_send_pbc(server, client, sender, msg);
}

#endif
