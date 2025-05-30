/*
** EPITECH PROJECT, 2025
** src/server/client/command_queue.c
** File description:
** Command queue management functions
*/

#define _GNU_SOURCE
#include "server/server.h"
#include <string.h>

void command_queue_init(command_queue_t *queue)
{
    if (!queue)
        return;
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
}

void command_queue_destroy(command_queue_t *queue)
{
    command_t *current;
    command_t *next;

    if (!queue)
        return;
    current = queue->head;
    while (current) {
        next = current->next;
        if (current->data)
            free(current->data);
        free(current);
        current = next;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->count = 0;
}

static command_t *create_command(const char *data, size_t length)
{
    command_t *cmd = malloc(sizeof(command_t));

    if (!cmd)
        return NULL;
    cmd->data = strndup(data, length);
    if (!cmd->data) {
        free(cmd);
        return NULL;
    }
    cmd->length = length;
    cmd->next = NULL;
    return cmd;
}

int command_queue_push(command_queue_t *queue, const char *data,
    size_t length)
{
    command_t *cmd;

    if (!queue || !data || queue->count >= MAX_COMMAND_QUEUE)
        return -1;
    cmd = create_command(data, length);
    if (!cmd)
        return -1;
    if (!queue->head) {
        queue->head = cmd;
        queue->tail = cmd;
    } else {
        queue->tail->next = cmd;
        queue->tail = cmd;
    }
    queue->count++;
    return 0;
}

command_t *command_queue_pop(command_queue_t *queue)
{
    command_t *cmd;

    if (!queue || !queue->head)
        return NULL;
    cmd = queue->head;
    queue->head = cmd->next;
    if (!queue->head)
        queue->tail = NULL;
    queue->count--;
    cmd->next = NULL;
    return cmd;
}
