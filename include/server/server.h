/*
** EPITECH PROJECT, 2025
** include/server/server.h
** File description:
** Enhanced Zappy server header with seeding support
*/

#ifndef SERVER_H_
    #define SERVER_H_

    #include <stddef.h>
    #include <stdint.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <sys/poll.h>
    #include <time.h>
    #include <stdbool.h>
    #include "shared/utils.h"

    #define MAX_CLIENTS 100
    #define BUFFER_SIZE 4096
    #define MAX_COMMAND_QUEUE 50
    #define MIN_MAP_SIZE 6
    #define MAX_MAP_SIZE 50
    #define SEEDER_MOVE_INTERVAL 5.0
    #define SEEDER_ELEVATION_INTERVAL 30.0
    #define SEEDER_ELEVATION_DURATION 5.0

typedef enum client_type_e {
    CLIENT_TYPE_UNKNOWN,
    CLIENT_TYPE_AI,
    CLIENT_TYPE_GRAPHIC
} client_type_t;

typedef enum resource_type_e {
    RESOURCE_FOOD,
    RESOURCE_LINEMATE,
    RESOURCE_DERAUMERE,
    RESOURCE_SIBUR,
    RESOURCE_MENDIANE,
    RESOURCE_PHIRAS,
    RESOURCE_THYSTAME,
    RESOURCE_COUNT
} resource_type_t;

typedef struct action_s action_t;

typedef struct client_s {
    int fd;
    client_type_t type;
    char *buffer;
    size_t buffer_size;
    size_t buffer_pos;
    bool is_authenticated;
    char *team_name;
    action_t *action_queue_head;
    action_t *action_queue_tail;
    size_t action_queue_count;
} client_t;

typedef struct tile_s {
    int resources[RESOURCE_COUNT];
    struct player_s **players;
    size_t player_count;
    size_t player_capacity;
} tile_t;

typedef struct player_s {
    int id;
    int x;
    int y;
    int orientation;
    int level;
    char *team_name;
    int resources[RESOURCE_COUNT];
    double last_action_time;
    bool is_elevating;
    double elevation_start_time;
} player_t;

typedef struct map_s {
    tile_t **tiles;
    int width;
    int height;
} map_t;

typedef struct seeder_state_s {
    player_t *player;
    double last_move_time;
    double last_elevation_time;
    int move_step;
    bool elevation_active;
} seeder_state_t;

typedef struct server_config_s {
    size_t port;
    size_t width;
    size_t height;
    size_t max_clients_per_team;
    size_t freq;
    char **team_names;
    size_t team_count;
    bool seed_mode;
} server_config_t;

typedef struct game_state_s {
    map_t *map;
    player_t **players;
    size_t player_count;
    size_t player_capacity;
    double current_time;
    int next_player_id;
    seeder_state_t *seeder;
} game_state_t;

typedef struct server_s {
    int server_fd;
    int signal_fd;
    server_config_t config;
    client_t *clients;
    size_t client_count;
    size_t client_capacity;
    struct pollfd *poll_fds;
    size_t poll_count;
    bool is_running;
    game_state_t *game;
} server_t;

int server_create(server_t *server, const server_config_t *config);
void server_run(server_t *server);
void server_destroy(server_t *server);
int parse_arguments(int argc, const char **argv, server_config_t *config);
int process_argument(const char **argv, int *i, int argc,
    server_config_t *config);
game_state_t *game_state_create(const server_config_t *config);
void game_state_destroy(game_state_t *game);
void game_state_update(game_state_t *game, double delta_time);
map_t *map_create(int width, int height);
int allocate_map_tiles(map_t *map);
void send_response(client_t *client, const char *response);
void map_destroy(map_t *map);
void map_place_resources(map_t *map);
tile_t *map_get_tile(const map_t *map, int x, int y);
player_t *player_create(int id, int x, int y, const char *team_name);
void player_destroy(player_t *player);
void player_move(player_t *player, map_t *map);
void player_set_position(player_t *player, map_t *map, int x, int y);
seeder_state_t *seeder_create(map_t *map);
void seeder_destroy(seeder_state_t *seeder);
void seeder_update(seeder_state_t *seeder, map_t *map, double current_time);
double get_current_time(void);
int client_add(server_t *server, int client_fd);
void client_remove(server_t *server, size_t index);
client_t *client_find_by_fd(server_t *server, int fd);
void client_authenticate(client_t *client, const char *message);
void protocol_handle_graphic_command(server_t *server, client_t *client,
    const char *cmd);
void protocol_handle_ai_command(server_t *server, client_t *client,
    const char *cmd);
void protocol_send_map_size(server_t *server, client_t *client);
void protocol_send_tile_content(server_t *server, client_t *client,
    int x, int y);
void protocol_send_player_info(server_t *server, client_t *client,
    const player_t *player);
void network_handle_events(server_t *server, int ready_count);
int signal_handler_init(void);
void signal_handler_cleanup(int signal_fd);
int signal_handler_check(int signal_fd);
void client_handle_message(server_t *server, client_t *client);

#endif /* !SERVER_H_ */
