/*
** EPITECH PROJECT, 2025
** src/server/protocol/ai_commands.c
** File description:
** AI command dispatch table implementation for protocol_ai
*/

#include "server/ai_commands.h"
#include "server/protocol_ai.h"

extern void handle_forward(server_t*, client_t*, const char*);
extern void handle_right(server_t*, client_t*, const char*);
extern void handle_left(server_t*, client_t*, const char*);
extern void handle_look(server_t*, client_t*, const char*);
extern void handle_inventory(server_t*, client_t*, const char*);
extern void handle_broadcast(server_t*, client_t*, const char*);
extern void handle_connect_nbr(server_t*, client_t*, const char*);
extern void handle_fork(server_t*, client_t*, const char*);
extern void handle_eject(server_t*, client_t*, const char*);
extern void handle_take(server_t*, client_t*, const char*);
extern void handle_set(server_t*, client_t*, const char*);
extern void handle_incantation(server_t*, client_t*, const char*);

const ai_cmd_t ai_commands[] = {
    {"Forward", 7, handle_forward},
    {"Right", 5, handle_right},
    {"Left", 4, handle_left},
    {"Look", 4, handle_look},
    {"Inventory", 9, handle_inventory},
    {"Broadcast ", 10, handle_broadcast},
    {"Connect_nbr", 11, handle_connect_nbr},
    {"Fork", 4, handle_fork},
    {"Eject", 5, handle_eject},
    {"Take ", 5, handle_take},
    {"Set ", 4, handle_set},
    {"Incantation", 11, handle_incantation},
};
const size_t ai_commands_count = sizeof(ai_commands) / sizeof(*ai_commands);
