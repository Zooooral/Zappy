/*
** EPITECH PROJECT, 2025
** src/server/server_run.c
** File description:
** Enhanced server main loop with time management
*/

#include "server/server.h"
#include "server/server_broadcast.h"
#include <errno.h>
#include <string.h>
#include <sys/time.h>

static void print_server_teams(const server_t *server)
{
    size_t i;

    printf("[SERVER] Teams: ");
    for (i = 0; i < server->config.team_count; i++) {
        printf("%s", server->config.team_names[i]);
        if (i < server->config.team_count - 1)
            printf(", ");
    }
    printf("\n");
}

static void print_server_info(const server_t *server)
{
    printf("[SERVER] Starting Enhanced Zappy server\n");
    printf("[SERVER] Port: %zu\n", server->config.port);
    printf("[SERVER] Map: %zux%zu\n", server->config.width,
        server->config.height);
    print_server_teams(server);
    printf("[SERVER] Time frequency: %zu\n", server->config.freq);
    if (server->config.seed_mode)
        printf("[SERVER] Running in SEED mode\n");
    printf("[SERVER] Listening for connections...\n\n");
}

static int check_for_shutdown_signal(server_t *server)
{
    if (signal_handler_check(server->signal_fd)) {
        printf("\n[SERVER] Shutdown signal received\n");
        server->is_running = false;
        return 1;
    }
    return 0;
}

static double calculate_delta_time(double *last_time)
{
    double current_time = get_current_time();
    double delta = current_time - *last_time;

    *last_time = current_time;
    return delta;
}

static int handle_poll_events(server_t *server)
{
    int ready = poll(server->poll_fds, server->poll_count, 16);

    if (ready == -1) {
        if (errno == EINTR)
            return 0;
        printf("[SERVER] Poll error: %s\n", strerror(errno));
        return -1;
    }
    if (ready > 0)
        network_handle_events(server, ready);
    return 0;
}

static void update_game_and_broadcast(server_t *server, double delta_time,
    double *broadcast_timer)
{
    if (server->game)
        game_state_update(server->game, delta_time);
    *broadcast_timer += delta_time;
    if (*broadcast_timer >= 0.1) {
        broadcast_seeder_updates(server);
        *broadcast_timer = 0.0;
    }
}

void server_run(server_t *server)
{
    double last_time = get_current_time();
    double broadcast_timer = 0.0;
    double delta_time;

    print_server_info(server);
    while (server->is_running) {
        delta_time = calculate_delta_time(&last_time);
        if (check_for_shutdown_signal(server))
            break;
        if (handle_poll_events(server) == -1)
            break;
        update_game_and_broadcast(server, delta_time, &broadcast_timer);
    }
    printf("[SERVER] Server shutting down\n");
}
