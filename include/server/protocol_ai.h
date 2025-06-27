/*
** EPITECH PROJECT, 2025
** include/server/protocol_ai.h
** File description:
** Protocol handling for AI clients
*/

#ifndef PROTOCOL_AI_H_
    #define PROTOCOL_AI_H_

    #include "server.h"
    #include "vision.h"

typedef enum {
    AI_ACTION_FORWARD,
    AI_ACTION_RIGHT,
    AI_ACTION_LEFT,
    AI_ACTION_LOOK,
    AI_ACTION_INVENTORY,
    AI_ACTION_BROADCAST,
    AI_ACTION_CONNECT_NBR,
    AI_ACTION_FORK,
    AI_ACTION_EJECT,
    AI_ACTION_TAKE,
    AI_ACTION_SET,
    AI_ACTION_INCANTATION,
    AI_ACTION_COUNT
} ai_action_type_t;


static const double ai_action_duration[] = {
    [AI_ACTION_FORWARD] = 7.0,
    [AI_ACTION_RIGHT] = 7.0,
    [AI_ACTION_LEFT] = 7.0,
    [AI_ACTION_LOOK] = 7.0,
    [AI_ACTION_INVENTORY] = 1.0,
    [AI_ACTION_BROADCAST] = 7.0,
    [AI_ACTION_CONNECT_NBR] = 0.0,
    [AI_ACTION_FORK] = 42.0,
    [AI_ACTION_EJECT] = 7.0,
    [AI_ACTION_TAKE] = 7.0,
    [AI_ACTION_SET] = 7.0,
    [AI_ACTION_INCANTATION] = 300.0
};

typedef struct ai_action_data_s {
    ai_action_type_t type;
    server_t *server;
    char *cmd;
} ai_action_data_t;

extern void handle_forward(server_t *, client_t *, const char *);
extern void handle_right(server_t *, client_t *, const char *);
extern void handle_left(server_t *, client_t *, const char *);
extern void handle_look(server_t *, client_t *, const char *);
extern void handle_inventory(server_t *, client_t *, const char *);
extern void handle_broadcast(server_t *, client_t *, const char *);
extern void handle_connect_nbr(server_t *, client_t *, const char *);
extern void handle_fork(server_t *, client_t *, const char *);
extern void handle_eject(server_t *, client_t *, const char *);
extern void handle_take(server_t *, client_t *, const char *);
extern void handle_set(server_t *, client_t *, const char *);
extern void handle_incantation(server_t *, client_t *, const char *);

void protocol_handle_ai_command(server_t *server, client_t *client,
    const char *cmd);
void create_and_queue_action(server_t *server, client_t *client,
    const char *cmd, const ai_action_type_t type);
void ai_callback_handler(client_t *client, void *data);

#endif /* !PROTOCOL_AI_H_ */
