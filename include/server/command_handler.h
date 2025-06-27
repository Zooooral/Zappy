/*
** EPITECH PROJECT, 2025
** include/server/command_handler.h
** File description:
** Command handling functions
*/

#ifndef COMMAND_HANDLER_H_
    #define COMMAND_HANDLER_H_

    #include "server.h"

void command_handle_graphic(server_t *server, client_t *client,
    const char *cmd);
void send_all_tiles(server_t *server, client_t *client);

#endif /* !COMMAND_HANDLER_H_ */
