/*
** EPITECH PROJECT, 2025
** include/server/command_queue.h
** File description:
** Command queue management functions
*/

#ifndef COMMAND_QUEUE_H_
    #define COMMAND_QUEUE_H_

    #include "server.h"

void command_queue_init(command_queue_t *queue);
void command_queue_destroy(command_queue_t *queue);
int command_queue_push(command_queue_t *queue, const char *data,
    size_t length);
command_t *command_queue_pop(command_queue_t *queue);

#endif /* !COMMAND_QUEUE_H_ */
