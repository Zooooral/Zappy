#ifndef AI_ACTIONS3_H
    #define AI_ACTIONS3_H
    #include <math.h>
    #include <string.h>

static inline void ai_action_connect_nbr(server_t *server, client_t *client, char *cmd)
{
    if (!server || !client || !client->team_name) {
        send_response(client, "ko\n");
        (void)cmd;
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

static inline void ai_action_broadcast(server_t *server, client_t *client, char *msg)
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

#endif
