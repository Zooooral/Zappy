/*
** EPITECH PROJECT, 2025
** include/server/egg_update.h
** File description:
** Egg update header for Zappy
*/

#ifndef EGG_UPDATE_H
    #define EGG_UPDATE_H

    #include "server/server.h"

void add_egg_timer(int egg_id, double current_time);
void update_eggs(server_t *server, double current_time);
void cleanup_egg_timers(void);

#endif // EGG_UPDATE_H
