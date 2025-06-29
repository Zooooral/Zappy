/*
** EPITECH PROJECT, 2025
** src/server/client/client_management.c
** File description:
** Enhanced client management with command queuing
*/

#include "server/egg.h"
#include "server/server.h"
#include "server/broadcast.h"
#include "server/egg_manager.h"
#include "server/dynamic_array.h"
#include <stdlib.h>
#include <string.h>

void egg_die(server_t *server, egg_t *egg)
{
    if (!server || !egg)
        return;
    broadcast_egg_died(server, egg->id);
    free(egg->team_name);
    free(egg);
}

void hatch_egg(server_t *server, egg_t *egg)
{
    if (!server || !egg || egg->hatched)
        return;
    egg->hatched = true;
    broadcast_egg_hatched(server, egg->id);
}

egg_t *create_egg(server_t *server, player_t *parent)
{
    static int egg_id_counter = 0;
    egg_t *egg = NULL;

    if (!server || !parent)
        return NULL;
    egg = malloc(sizeof(egg_t));
    if (!egg)
        return NULL;
    egg->id = egg_id_counter;
    ++egg_id_counter;
    egg->team_name = strdup(parent->team_name);
    egg->x = parent->x;
    egg->y = parent->y;
    egg->hatched = false;
    egg->connected = false;
    broadcast_egg_laid(server, egg->id, parent);
    return egg;
}
