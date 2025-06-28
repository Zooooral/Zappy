/*
** EPITECH PROJECT, 2025
** include/server/egg_manager.h
** File description:
** Egg manager header for Zappy
*/

#ifndef EGG_MANAGER_H
    #define EGG_MANAGER_H

    #include "server/server.h"
    #include "server/egg.h"

egg_t *egg_manager_add_egg(server_t *server, player_t *parent);
size_t egg_manager_get_available_count(server_t *server,
    const char *team_name);
egg_t *egg_manager_find_available_egg(server_t *server, const char *team_name);
void egg_manager_remove_egg(server_t *server, egg_t *egg);

#endif // EGG_MANAGER_H
