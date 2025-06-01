/*
** EPITECH PROJECT, 2025
** include/server/time.h
** File description:
** Time management header for Zappy
*/

#ifndef TIME_H
    #define TIME_H

    #include <stdint.h>

    #include "server/server.h"

typedef struct action_s {
    char *command;
    double exec_time;
    void (*callback)(client_t *client, void *data);
    void *data;
    struct action_s *next;
} action_t;

double get_time_unit(server_t *server);
void queue_action(client_t *client, action_t *action);
void process_actions(server_t *server);

#endif // TIME_H
