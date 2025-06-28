/*
** EPITECH PROJECT, 2025
** coding/zappy/src/server/client/client_management_helper
** File description:
** you know what this is
*/

#ifndef CLIENT_MANAGEMENT_HELPER_H
    #define CLIENT_MANAGEMENT_HELPER_H

static inline int initialize_client_buffer(client_t *client)
{
    client->buffer_size = BUFFER_SIZE;
    client->buffer = malloc(client->buffer_size);
    if (!client->buffer)
        return -1;
    client->buffer_pos = 0;
    return 0;
}
#endif // CLIENT_MANAGEMENT_HELPER_H
