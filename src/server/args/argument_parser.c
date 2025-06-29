/*
** EPITECH PROJECT, 2025
** src/server/args/argument_parser.c
** File description:
** Enhanced command line argument parsing
*/

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "server/server.h"

static int validate_config(const server_config_t *config)
{
    if (config->port == 0)
        return -1;
    if (config->width == 0)
        return -1;
    if (config->height == 0)
        return -1;
    if (config->max_clients_per_team == 0)
        return -1;
    if (config->freq == 0)
        return -1;
    if (config->team_count == 0)
        return -1;
    if (config->port < 1024 || config->port > 65535)
        return -1;
    return 0;
}

int parse_arguments(int argc, const char **argv, server_config_t *config)
{
    memset(config, 0, sizeof(server_config_t));
    config->refill_tiles = true;
    for (int i = 1; i < argc; i++) {
        if (process_argument(argv, &i, argc, config) == -1)
            return -1;
    }
    return validate_config(config);
}
