/*
** EPITECH PROJECT, 2025
** src/server/args/argument_handler.c
** File description:
** Command line argument processing functions
*/

#include "server/server.h"
#include <string.h>
#include <stdlib.h>

static int parse_numeric_arg(const char **argv, int *i, size_t *value)
{
    if (!argv[*i + 1])
        return -1;
    *value = atoi(argv[*i + 1]);
    (*i)++;
    return (*value > 0) ? 0 : -1;
}

static int parse_team_names(const char **argv, int *i, int argc,
    server_config_t *config)
{
    int start = *i + 1;
    int count = 0;

    while (*i + 1 < argc && argv[*i + 1][0] != '-') {
        count++;
        (*i)++;
    }
    if (count == 0)
        return -1;
    config->team_names = (char **)&argv[start];
    config->team_count = count;
    return 0;
}

static void setup_seed_config(server_config_t *config)
{
    static char *seed_teams[] = {"Seeders"};

    srand(time(NULL));
    config->width = MIN_MAP_SIZE + rand() %
        (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    config->height = MIN_MAP_SIZE + rand() %
        (MAX_MAP_SIZE - MIN_MAP_SIZE + 1);
    config->team_names = seed_teams;
    config->team_count = 1;
    config->max_clients_per_team = 1;
    config->freq = 100;
    if (config->port == 0)
        config->port = 4242;
}

static int process_port_arg(const char **argv, int *i,
    server_config_t *config)
{
    return parse_numeric_arg(argv, i, &config->port);
}

static int process_width_arg(const char **argv, int *i,
    server_config_t *config)
{
    return config->seed_mode ? 0 : parse_numeric_arg(argv, i,
        &config->width);
}

static int process_height_arg(const char **argv, int *i,
    server_config_t *config)
{
    return config->seed_mode ? 0 : parse_numeric_arg(argv, i,
        &config->height);
}

static int process_clients_arg(const char **argv, int *i,
    server_config_t *config)
{
    return config->seed_mode ? 0 : parse_numeric_arg(argv, i,
        &config->max_clients_per_team);
}

static int process_freq_arg(const char **argv, int *i,
    server_config_t *config)
{
    return config->seed_mode ? 0 : parse_numeric_arg(argv, i,
        &config->freq);
}

static int process_teams_arg(const char **argv, int *i, int argc,
    server_config_t *config)
{
    return config->seed_mode ? 0 : parse_team_names(argv, i, argc, config);
}

int process_argument(const char **argv, int *i, int argc,
    server_config_t *config)
{
    if (strcmp(argv[*i], "--seed") == 0) {
        config->seed_mode = true;
        setup_seed_config(config);
        return 0;
    }
    if (strcmp(argv[*i], "-p") == 0)
        return process_port_arg(argv, i, config);
    if (strcmp(argv[*i], "-x") == 0)
        return process_width_arg(argv, i, config);
    if (strcmp(argv[*i], "-y") == 0)
        return process_height_arg(argv, i, config);
    if (strcmp(argv[*i], "-c") == 0)
        return process_clients_arg(argv, i, config);
    if (strcmp(argv[*i], "-f") == 0)
        return process_freq_arg(argv, i, config);
    if (strcmp(argv[*i], "-n") == 0)
        return process_teams_arg(argv, i, argc, config);
    return -1;
}
