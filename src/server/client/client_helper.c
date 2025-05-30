/*
** EPITECH PROJECT, 2025
** src/server/client/client_helper.c
** File description:
** Client helper functions
*/

#include "server/server.h"
#include <sys/socket.h>
#include <string.h>
#include <errno.h>

static int expand_client_buffer(client_t *client, ssize_t received)
{
    if (client->buffer_pos + received >= client->buffer_size) {
        client->buffer_size *= 2;
        client->buffer = realloc(client->buffer, client->buffer_size);
        if (!client->buffer)
            return -1;
    }
    return 0;
}

static ssize_t receive_raw_data(client_t *client, char *buffer)
{
    ssize_t received = recv(client->fd, buffer, 1023, MSG_DONTWAIT);

    if (received == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
            return 0;
        return -1;
    }
    if (received == 0)
        return -1;
    buffer[received] = '\0';
    return received;
}

static int receive_client_data(client_t *client)
{
    char buffer[1024];
    ssize_t received = receive_raw_data(client, buffer);

    if (received <= 0)
        return received;
    if (expand_client_buffer(client, received) == -1)
        return -1;
    memcpy(client->buffer + client->buffer_pos, buffer, received);
    client->buffer_pos += received;
    return 1;
}

static void process_command(server_t *server, client_t *client,
    const char *command)
{
    if (!client->is_authenticated) {
        client_authenticate(client, command);
        return;
    }
    if (client->type == CLIENT_TYPE_GRAPHIC) {
        protocol_handle_graphic_command(server, client, command);
    } else {
        protocol_handle_ai_command(server, client, command);
    }
}

static void process_message_line(server_t *server, client_t *client,
    char *newline)
{
    size_t msg_len = newline - client->buffer;

    *newline = '\0';
    process_command(server, client, client->buffer);
    memmove(client->buffer, newline + 1, client->buffer_pos - msg_len - 1);
    client->buffer_pos -= msg_len + 1;
}

static void handle_receive_error(server_t *server, client_t *client)
{
    size_t client_index = client - server->clients;

    client_remove(server, client_index);
}

static void process_received_messages(server_t *server, client_t *client)
{
    char *newline = memchr(client->buffer, '\n', client->buffer_pos);

    while (newline) {
        process_message_line(server, client, newline);
        newline = memchr(client->buffer, '\n', client->buffer_pos);
    }
}

void client_handle_message(server_t *server, client_t *client)
{
    int recv_result = receive_client_data(client);

    if (recv_result == -1) {
        handle_receive_error(server, client);
        return;
    }
    if (recv_result == 0)
        return;
    process_received_messages(server, client);
}
