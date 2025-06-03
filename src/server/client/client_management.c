/*
** EPITECH PROJECT, 2025
** src/server/client/client_management.c
** File description:
** Enhanced client management with command queuing
*/

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "server/server.h"
#include "server/time.h"

static int initialize_client_buffer(client_t *client)
{
    client->buffer_size = BUFFER_SIZE;
    client->buffer = malloc(client->buffer_size);
    if (!client->buffer)
        return -1;
    client->buffer_pos = 0;
    return 0;
}

static int setup_client_connection(client_t *client,
    int client_fd)
{
    client->fd = client_fd;
    client->type = CLIENT_TYPE_UNKNOWN;
    client->is_authenticated = false;
    client->team_name = NULL;
    if (initialize_client_buffer(client) == -1) {
        close(client_fd);
        return -1;
    }
    client->action_queue_head = NULL;
    client->action_queue_tail = NULL;
    client->action_queue_count = 0;
    return 0;
}

static void update_poll_fds(server_t *server, int client_fd)
{
    server->poll_fds[server->poll_count].fd = client_fd;
    server->poll_fds[server->poll_count].events = POLLIN;
    server->client_count++;
    server->poll_count++;
}

int client_add(server_t *server, int client_fd)
{
    client_t *client;
    ssize_t sent;

    if (!server || server->client_count >= server->client_capacity)
        return -1;
    client = &server->clients[server->client_count];
    if (setup_client_connection(client, client_fd) == -1)
        return -1;
    update_poll_fds(server, client_fd);
    sent = send(client_fd, "WELCOME\n", 8, 0);
    (void)sent;
    return 0;
}

static void cleanup_client_resources(client_t *client)
{
    if (client->fd != -1)
        close(client->fd);
    if (client->buffer)
        free(client->buffer);
    if (client->team_name)
        free(client->team_name);
    // Free all actions in the action queue
    action_t *action = client->action_queue_head;
    while (action) {
        action_t *next = action->next;
        free(action->command);
        free(action);
        action = next;
    }
    client->action_queue_head = NULL;
    client->action_queue_tail = NULL;
    client->action_queue_count = 0;
}

static void shift_clients_array(server_t *server, size_t index)
{
    size_t i;

    for (i = index; i < server->client_count - 1; i++)
        server->clients[i] = server->clients[i + 1];
}

static void update_poll_fds_after_removal(server_t *server)
{
    size_t i;

    for (i = 1; i <= server->client_count; i++) {
        server->poll_fds[i].fd = server->clients[i - 1].fd;
        server->poll_fds[i].events = POLLIN;
    }
    server->poll_count = server->client_count + 1;
}

void client_remove(server_t *server, size_t index)
{
    if (!server || index >= server->client_count)
        return;
    cleanup_client_resources(&server->clients[index]);
    shift_clients_array(server, index);
    server->client_count--;
    update_poll_fds_after_removal(server);
}

client_t *client_find_by_fd(server_t *server, int fd)
{
    size_t i;

    if (!server)
        return NULL;
    for (i = 0; i < server->client_count; i++) {
        if (server->clients[i].fd == fd)
            return &server->clients[i];
    }
    return NULL;
}

void client_authenticate(client_t *client, const char *message)
{
    ssize_t sent;

    if (!client || !message)
        return;
    if (strcmp(message, "GRAPHIC") == 0) {
        client->type = CLIENT_TYPE_GRAPHIC;
        client->is_authenticated = true;
        sent = send(client->fd, "msz 10 10\n", 10, 0);
        (void)sent;
    } else {
        client->type = CLIENT_TYPE_AI;
        client->team_name = strdup(message);
        client->is_authenticated = true;
    }
}
