/*
** EPITECH PROJECT, 2025
** src/server/server_cleanup.c
** File description:
** Enhanced server cleanup functions
*/

#include "server/server.h"
#include "server/time.h"
#include <unistd.h>

static void clean_action_queue(client_t *client)
{
    action_t *action = client->action_queue_head;
    action_t *next;

    while (action) {
        next = action->next;
        free(action->command);
        free(action);
        action = next;
    }
    client->action_queue_head = NULL;
    client->action_queue_tail = NULL;
    client->action_queue_count = 0;
}

static void close_client_connection(client_t *client)
{
    if (!client)
        return;
    if (client->fd != -1) {
        close(client->fd);
        client->fd = -1;
    }
    if (client->buffer) {
        free(client->buffer);
        client->buffer = NULL;
    }
    if (client->team_name) {
        free(client->team_name);
        client->team_name = NULL;
    }
    clean_action_queue(client);
}

static void cleanup_all_clients(server_t *server)
{
    size_t i;

    if (!server->clients)
        return;
    for (i = 0; i < server->client_count; i++)
        close_client_connection(&server->clients[i]);
    free(server->clients);
    server->clients = NULL;
}

static void cleanup_server_socket(server_t *server)
{
    if (server->server_fd != -1) {
        close(server->server_fd);
        server->server_fd = -1;
    }
}

static void cleanup_game_state(server_t *server)
{
    if (server->game) {
        game_state_destroy(server->game);
        server->game = NULL;
    }
}

static void cleanup_poll_fds(server_t *server)
{
    if (server->poll_fds) {
        free(server->poll_fds);
        server->poll_fds = NULL;
    }
}

void server_destroy(server_t *server)
{
    if (!server)
        return;
    printf("[SERVER] Cleaning up server resources\n");
    cleanup_all_clients(server);
    cleanup_server_socket(server);
    signal_handler_cleanup(server->signal_fd);
    cleanup_game_state(server);
    cleanup_poll_fds(server);
    server->is_running = false;
    printf("[SERVER] Cleanup completed\n");
}
