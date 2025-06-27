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
#include "server/broadcast.h"
#include "server/protocol_graphic.h"
#include "server/payloads.h"
#include "server/command_handler.h"

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
    action_t *action;
    action_t *next;

    if (client->fd != -1)
        close(client->fd);
    if (client->buffer)
        free(client->buffer);
    if (client->team_name)
        free(client->team_name);
    action = client->action_queue_head;
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

static void shift_clients_array(server_t *server, size_t index)
{
    for (size_t i = index; i < server->client_count - 1; ++i)
        server->clients[i] = server->clients[i + 1];
}

static void update_poll_fds_after_removal(server_t *server)
{
    for (size_t i = 1; i <= server->client_count; ++i) {
        server->poll_fds[i].fd = server->clients[i - 1].fd;
        server->poll_fds[i].events = POLLIN;
    }
    server->poll_count = server->client_count + 1;
}

void client_remove(server_t *server, size_t index)
{
    player_t *player;

    if (!server || index >= server->client_count)
        return;
    player = server->clients[index].player;
    if (player) {
        broadcast_message_to_guis(server, player, gui_payload_pdi);
    }
    cleanup_client_resources(&server->clients[index]);
    shift_clients_array(server, index);
    server->client_count--;
    update_poll_fds_after_removal(server);
}

client_t *client_find_by_fd(server_t *server, int fd)
{
    if (!server)
        return NULL;
    for (size_t i = 0; i < server->client_count; ++i) {
        if (server->clients[i].fd == fd)
            return &server->clients[i];
    }
    return NULL;
}

static void client_validate(server_t *server, client_t *client, const char *message)
{
    char response[128];
    int pos[2] = {rand() % server->config.width, rand() % server->config.height};

    client->player = NULL;
    client->type = CLIENT_TYPE_AI;
    client->team_name = strdup(message);
    client->is_authenticated = true;
    if (server->game->player_count < server->game->player_capacity) {
        client->player = player_create(client, pos[0], pos[1], message);
        printf("[DEBUG] AI client created player: %p\n", (void*)client->player);
        if (client->player) {
            player_set_position(server, client->player, pos[0], pos[1]);
            add_player_to_game(server->game, client->player);
            broadcast_message_to_guis(server, client->player, gui_payload_pnw);
        }
    }
    snprintf(response, sizeof(response), "%ld\n",
        server->config.max_clients_per_team);
    send_response(client, response);
    snprintf(response, sizeof(response), "%ld %ld\n", server->config.width,
        server->config.height);
    send_response(client, response);
}

void client_authenticate(server_t *server, client_t *client, const char *message)
{
    if (!server || !client || !message)
        return;
    if (strcmp(message, "GRAPHIC") != 0) {
        printf("AI client authenticated with team: %s\n", message);
        client_validate(server, client, message);
        return;
    }
    client->type = CLIENT_TYPE_GRAPHIC;
    client->is_authenticated = true;
    protocol_send_map_size(server, client);
    send_all_tiles(server, client);
    for (size_t i = 0; i < server->client_count; ++i) {
        if (server->clients[i].type == CLIENT_TYPE_AI &&
            server->clients[i].player != NULL) {
            protocol_send_player_info(client, server->clients[i].player);
        }
    }
}
