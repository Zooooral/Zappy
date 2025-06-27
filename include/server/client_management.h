/*
** EPITECH PROJECT, 2025
** include/server/client_management.h
** File description:
** Client management functions
*/

#ifndef CLIENT_MANAGEMENT_H_
    #define CLIENT_MANAGEMENT_H_

    #include <netinet/in.h>

    #include "server.h"

void client_remove(server_t *server, size_t index);
client_t *client_find_by_fd(server_t *server, int fd);

#endif /* !CLIENT_MANAGEMENT_H_ */
