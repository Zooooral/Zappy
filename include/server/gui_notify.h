/*
** EPITECH PROJECT, 2025
** include/server/gui_notify.h
** File description:
** GUI notification header for Zappy
*/

#ifndef GUI_NOTIFY_H
    #define GUI_NOTIFY_H

    #include "server/server.h"

void gui_notify_pnw(client_t *client);
void gui_notify_ppo(client_t *client);
void gui_notify_plv(client_t *client);
void gui_notify_pin(client_t *client);
void gui_notify_action(const char *action, client_t *client);
void gui_notify_egg(const char *event, void *egg_data);
void gui_notify_time(double time_unit);
void gui_notify_game_end(const char *team_name);

#endif // GUI_NOTIFY_H
