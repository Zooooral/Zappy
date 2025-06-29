/*
** EPITECH PROJECT, 2025
** src/server/egg_manager.c
** File description:
** Egg management implementation for Zappy
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "server/server.h"
#include "server/egg.h"
#include "server/broadcast.h"
#include "server/dynamic_array.h"

egg_t *egg_manager_add_egg(server_t *server, player_t *parent)
{
    egg_t *egg = NULL;

    if (!server || !parent)
        return NULL;
    egg = create_egg(server, parent);
    if (!egg)
        return NULL;
    server->eggs = da_push(server->eggs, &egg, sizeof(egg_t *));
    return egg;
}

size_t egg_manager_get_available_count(server_t *server, const char *team_name)
{
    size_t available = 0;
    egg_t **eggs = NULL;

    if (!server || !server->eggs || !team_name)
        return 0;
    eggs = server->eggs;
    for (size_t i = 0; i < DA_LEN(eggs); i++) {
        if (eggs[i] && eggs[i]->team_name &&
            strcmp(eggs[i]->team_name, team_name) == 0 &&
            eggs[i]->hatched && !eggs[i]->connected) {
            available++;
        }
    }
    return available;
}

egg_t *egg_manager_find_available_egg(server_t *server, const char *team_name)
{
    egg_t **eggs = NULL;

    if (!server || !server->eggs || !team_name)
        return NULL;
    eggs = (egg_t **)server->eggs;
    for (size_t i = 0; i < DA_LEN(eggs); i++) {
        if (eggs[i] && eggs[i]->team_name &&
            strcmp(eggs[i]->team_name, team_name) == 0 &&
            eggs[i]->hatched && !eggs[i]->connected) {
            return eggs[i];
        }
    }
    return NULL;
}

void egg_manager_remove_egg(server_t *server, egg_t *egg)
{
    egg_t **eggs = NULL;
    size_t egg_count;

    if (!server || !server->eggs || !egg)
        return;
    eggs = server->eggs;
    egg_count = DA_LEN(eggs);
    for (size_t i = 0; i < egg_count; i++) {
        if (eggs[i] && eggs[i]->id == egg->id) {
            eggs[i] = eggs[egg_count - 1];
            da_pop(server->eggs, sizeof(egg_t *));
            return;
        }
    }
}
