/*
** EPITECH PROJECT, 2025
** src/server/network/network_handler.c
** File description:
** Enhanced network event handling with buffering
*/

#include "server/server.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

static void handle_new_connection(server_t *server)
{
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(server->server_fd,
        (struct sockaddr *)&client_addr, &addr_len);

    if (client_fd == -1) {
        printf("[SERVER] Accept failed: %s\n", strerror(errno));
        return;
    }
    if (client_add(server, client_fd) == -1) {
        printf("[SERVER] Failed to add client\n");
        close(client_fd);
    }
}

static bool should_remove_client(struct pollfd *pfd)
{
    if (pfd->revents & (POLLHUP | POLLERR | POLLNVAL)) {
        printf("[SERVER] Client fd:%d has error condition (revents: %d)\n",
            pfd->fd, pfd->revents);
        return true;
    }
    return false;
}

static void handle_client_input(server_t *server, client_t *client)
{
    client_handle_message(server, client);
    client = client_find_by_fd(server, client->fd);
}

static void handle_client_event(server_t *server, size_t poll_index)
{
    struct pollfd *pfd = &server->poll_fds[poll_index];
    client_t *client = client_find_by_fd(server, pfd->fd);
    bool should_remove = false;
    size_t client_index;

    if (!client)
        return;
    if (should_remove_client(pfd)) {
        should_remove = true;
    }
    if (pfd->revents & POLLIN && !should_remove_client(pfd)) {
        handle_client_input(server, client);
        if (!client_find_by_fd(server, pfd->fd))
            should_remove = false;
    }
    if (should_remove) {
        client_index = client - server->clients;
        client_remove(server, client_index);
    }
}

static void process_ready_events(server_t *server, int ready_count)
{
    size_t i;

    for (i = 1; i < server->poll_count && ready_count > 0; i++) {
        if (server->poll_fds[i].revents != 0) {
            handle_client_event(server, i);
            ready_count--;
        }
    }
}

void network_handle_events(server_t *server, int ready_count)
{
    if (!server)
        return;
    if (server->poll_fds[0].revents & POLLIN) {
        handle_new_connection(server);
        ready_count--;
    }
    process_ready_events(server, ready_count);
}
