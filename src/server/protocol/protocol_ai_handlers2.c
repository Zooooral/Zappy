/*
** EPITECH PROJECT, 2025
** src/server/protocol/protocol_ai_handlers2.c
** File description:
** AI protocol handlers (2/3)
*/

#include "server/server.h"
#include "server/protocol_ai.h"
#include "server/vision.h"
#include "server/incantation.h"
#include "server/broadcast.h"
#include "server/time.h"
#include "server/resource.h"
#include "server/lifecycle.h"
#include "server/gui_notify.h"
#include <math.h>
#include <string.h>


void handle_broadcast(server_t *server, client_t *client, const char *msg)
{
    if (!server || !client || !msg) {
        send_response(client, "ko\n");
        return;
    }
    player_t *sender = client->player;
    if (!sender) {
        send_response(client, "ko\n");
        return;
    }
    int width = server->game->map->width;
    int height = server->game->map->height;
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *other = &server->clients[i];
        if (other->type == CLIENT_TYPE_AI && other->fd != client->fd && other->player) {
            int dx = sender->x - other->player->x;
            int dy = sender->y - other->player->y;
            if (dx > width / 2) dx -= width;
            if (dx < -width / 2) dx += width;
            if (dy > height / 2) dy -= height;
            if (dy < -height / 2) dy += height;
            int direction = 0;
            if (dx == 0 && dy == 0) {
                direction = 0;
            } else {
                double angle = atan2(-(double)dy, (double)dx) * 180.0 / M_PI;
                if (angle < 0) angle += 360.0;
                if (angle >= 337.5 || angle < 22.5) direction = 3; // E
                else if (angle >= 22.5 && angle < 67.5) direction = 2; // NE
                else if (angle >= 67.5 && angle < 112.5) direction = 1; // N
                else if (angle >= 112.5 && angle < 157.5) direction = 8; // NW
                else if (angle >= 157.5 && angle < 202.5) direction = 7; // W
                else if (angle >= 202.5 && angle < 247.5) direction = 6; // SW
                else if (angle >= 247.5 && angle < 292.5) direction = 5; // S
                else if (angle >= 292.5 && angle < 337.5) direction = 4; // SE
                int orient = other->player->orientation;
                if (orient < 1 || orient > 4) orient = 1;
                direction = ((direction - 1 + 2 * (orient - 1)) % 8) + 1;
            }
            char *ai_msg = NULL;
            asprintf(&ai_msg, "message %d,%s\n", direction, msg);
            send_response(other, ai_msg);
            free(ai_msg);
        }
    }
    char *buf = NULL;
    asprintf(&buf, "pbc #%d %s\n", sender->id, msg);
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *other = &server->clients[i];
        if (other->type == CLIENT_TYPE_GRAPHIC) {
            send_response(other, buf);
        }
    }
    free(buf);
    send_response(client, "ok\n");
}

void handle_connect_nbr(server_t *server, client_t *client, const char *arg)
{
    if (!server || !client || !client->team_name) {
        send_response(client, "ko\n");
        return;
    }
    int available = (int)server->config.max_clients_per_team;
    int current = 0;
    for (size_t i = 0; i < server->game->player_count; ++i) {
        player_t *p = server->game->players[i];
        if (p && p->team_name && strcmp(p->team_name, client->team_name) == 0)
            current++;
    }
    available -= current;
    if (available < 0) available = 0;
    char *buf = NULL;
    asprintf(&buf, "%d\n", available);
    send_response(client, buf);
    free(buf);
}

void handle_fork(server_t *server, client_t *client, const char *arg)
{
    return;
}

void handle_eject(server_t *server, client_t *client, const char *arg)
{
    if (!server || !client || !client->player) {
        send_response(client, "ko\n");
        return;
    }
    int ejected = 0;
    player_t *self = client->player;
    int dx = 0, dy = 0;
    switch (self->orientation) {
        case 1: dy = -1; break; // North
        case 2: dx = 1; break;  // East
        case 3: dy = 1; break;  // South
        case 4: dx = -1; break; // West
        default: break;
    }
    int width = server->game->map->width;
    int height = server->game->map->height;
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *other = &server->clients[i];
        if (other != client && other->type == CLIENT_TYPE_AI && other->player &&
            other->player->x == self->x && other->player->y == self->y) {
            other->player->x = (other->player->x + dx + width) % width;
            other->player->y = (other->player->y + dy + height) % height;
            ejected++;
        }
    }
    char *buf = NULL;
    asprintf(&buf, "%s\n", ejected > 0 ? "ok" : "ko");
    send_response(client, buf);
    free(buf);
}

void handle_take(server_t *server, client_t *client, const char *resource)
{
    player_t *p = NULL;
    int resource_id = atoi(resource);

    if (!server || !client)
        return;
    for (int i = 0; i < server->game->player_count; ++i) {
        if (server->game->players[i]->id == client->fd) {
            p = server->game->players[i];
            break;
        }
    }
    if (!p || take_resource(client, server->game->map, resource_id) == -1) {
        return send_response(client, "ko\n");
    }
    ++p->resources[resource_id];
    send_response(client, "ok\n");
    char *pgt = NULL;
    asprintf(&pgt, "pgt #%d %d\n", p->id, resource_id);
    for (size_t i = 0; i < server->client_count; ++i) {
        client_t *gui = &server->clients[i];
        if (gui->type == CLIENT_TYPE_GRAPHIC) {
            send_response(gui, pgt);
        }
    }
    free(pgt);
    return;
}
