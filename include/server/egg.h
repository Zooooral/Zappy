/*
** EPITECH PROJECT, 2025
** include/server/egg.h
** File description:
** Egg handling header for Zappy
*/

#ifndef EGG_H
    #define EGG_H

    #include <stdbool.h>
    #include "server/server.h"
    #include "dynamic_array.h"
    #include "string.h"

typedef struct egg_s {
    int id;
    char *team_name;
    int x;
    int y;
    bool hatched;
    bool connected;
} egg_t;

// Create a new egg on the map at the player's position
egg_t *create_egg(server_t *server, player_t *parent);

// Handle egg hatching (when the timer expires)
void hatch_egg(server_t *server, egg_t *egg);

// Connect player to egg when they join a team
bool connect_to_egg(server_t *server, egg_t *egg, client_t *client);

// Free and clean up an egg when it dies
void egg_die(server_t *server, egg_t *egg);

// Function prototypes for egg management
egg_t *egg_manager_find_available_egg(server_t *server, const char *team_name);
void egg_manager_remove_egg(server_t *server, egg_t *egg);
size_t egg_manager_get_available_count(server_t *server,
    const char *team_name);
egg_t *egg_manager_add_egg(server_t *server, player_t *parent);
void egg_die(server_t *server, egg_t *egg);
void hatch_egg(server_t *server, egg_t *egg);
void add_egg_timer(int egg_id, double current_time);
egg_t *create_egg(server_t *server, player_t *parent);

static inline int egg_manager_count_eggs(const server_t *server,
    char const *team_name)
{
    int count = 0;

    if (!server || !server->eggs)
        return 0;
    for (size_t i = 0; i < DA_LEN(server->eggs); ++i) {
        if (strcmp(server->eggs[i]->team_name, team_name) == 0) {
            count++;
        }
    }
    return count;
}

#endif // EGG_H
