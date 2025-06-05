/*
** EPITECH PROJECT, 2025
** include/server/ai_commands.h
** File description:
** AI command dispatch table for protocol_ai
*/

#ifndef AI_COMMANDS_H
    #define AI_COMMANDS_H

    #include "server/server.h"
    #include "server/protocol_ai.h"

typedef void (*ai_cmd_fn_t)(server_t*, client_t*, const char*);

typedef struct {
    const char *name;
    int len;
    ai_cmd_fn_t fn;
} ai_cmd_t;

extern const ai_cmd_t ai_commands[];
extern const size_t ai_commands_count;

#endif // AI_COMMANDS_H
