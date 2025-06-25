/*
** EPITECH PROJECT, 2025
** src/server/server_init.c
** File description:
** Enhanced server initialization with game state
*/

#include "server/server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static int setup_socket_options(int fd)
{
    int opt = 1;

    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(fd);
        return -1;
    }
    return 0;
}

static int bind_socket_to_port(int fd, size_t port)
{
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(fd);
        return -1;
    }
    return 0;
}

static int create_server_socket(size_t port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    if (fd == -1)
        return -1;
    if (setup_socket_options(fd) == -1)
        return -1;
    if (bind_socket_to_port(fd, port) == -1)
        return -1;
    if (listen(fd, 10) == -1) {
        close(fd);
        return -1;
    }
    return fd;
}

static int allocate_clients_array(server_t *server)
{
    server->client_capacity = MAX_CLIENTS;
    server->clients = calloc(server->client_capacity, sizeof(client_t));
    if (!server->clients)
        return -1;
    return 0;
}

static int allocate_poll_fds_array(server_t *server)
{
    server->poll_fds = calloc(server->client_capacity + 1,
        sizeof(struct pollfd));
    if (!server->poll_fds) {
        free(server->clients);
        server->clients = NULL;
        return -1;
    }
    return 0;
}

static int allocate_server_memory(server_t *server)
{
    if (allocate_clients_array(server) == -1)
        return -1;
    if (allocate_poll_fds_array(server) == -1)
        return -1;
    return 0;
}

static void initialize_clients(server_t *server)
{
    size_t i;
    client_t *client;

    for (i = 0; i < server->client_capacity; i++) {
        client = &server->clients[i];
        client->fd = -1;
        client->type = CLIENT_TYPE_UNKNOWN;
        client->buffer = NULL;
        client->buffer_size = 0;
        client->buffer_pos = 0;
        client->is_authenticated = false;
        client->team_name = NULL;
        client->action_queue_head = NULL;
        client->action_queue_tail = NULL;
        client->action_queue_count = 0;
    }
}

static void setup_initial_poll_state(server_t *server)
{
    server->client_count = 0;
    server->poll_count = 1;
    server->is_running = true;
    server->poll_fds[0].fd = server->server_fd;
    server->poll_fds[0].events = POLLIN;
}

static int initialize_server_components(server_t *server,
    const server_config_t *config)
{
    server->signal_fd = signal_handler_init();
    if (server->signal_fd == -1)
        return -1;
    server->server_fd = create_server_socket(config->port);
    if (server->server_fd == -1) {
        signal_handler_cleanup(server->signal_fd);
        return -1;
    }
    return 0;
}

int server_create(server_t *server, const server_config_t *config)
{
    if (!server || !config)
        return -1;
    memset(server, 0, sizeof(server_t));
    server->config = *config;
    if (initialize_server_components(server, config) == -1)
        return -1;
    if (allocate_server_memory(server) == -1) {
        close(server->server_fd);
        signal_handler_cleanup(server->signal_fd);
        return -1;
    }
    setup_initial_poll_state(server);
    initialize_clients(server);
    server->game = game_state_create(server, &server->config);
    if (!server->game) {
        server_destroy(server);
        return -1;
    }
    return 0;
}
