/*
** EPITECH PROJECT, 2025
** src/server/protocol/ai_commands.c
** File description:
** AI command dispatch table implementation for protocol_ai
*/

#include "server/ai_commands.h"
#include "server/protocol_ai.h"

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
