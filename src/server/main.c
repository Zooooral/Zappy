/*
** EPITECH PROJECT, 2025
** src/server/main.c
** File description:
** Enhanced Zappy server main file
*/

#include "server/server.h"
#include "shared/utils.h"
#include <stdio.h>
#include <string.h>

static void print_help(const char *program_name)
{
    printf("USAGE: %s -p port -x width -y height -n name1 name2 "
        "... -c clientsNb -f freq\n", program_name);
    printf("  -p port        : port number\n");
    printf("  -x width       : width of the world\n");
    printf("  -y height      : height of the world\n");
    printf("  -n name1 name2 : name of the team\n");
    printf("  -c clientsNb   : number of authorized clients per team\n");
    printf("  -f freq        : reciprocal "
        "of time unit for execution of actions\n");
    printf("  -r <bool>      : refill tiles with resources\n");
}

static int handle_arguments(int argc, const char **argv,
    server_config_t *config)
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        return SUCCESS;
    }
    if (parse_arguments(argc, argv, config) == -1) {
        fprintf(stderr, "Error: Invalid arguments\n");
        print_help(argv[0]);
        return FAILURE;
    }
    return -1;
}

static int create_and_run_server(const server_config_t *config)
{
    server_t server = {0};

    if (server_create(&server, config) == -1) {
        fprintf(stderr, "Error: Failed to create server\n");
        return FAILURE;
    }
    server_run(&server);
    server_destroy(&server);
    return SUCCESS;
}

int main(const int argc, const char **argv)
{
    server_config_t config;
    int arg_result = handle_arguments(argc, argv, &config);

    if (arg_result != -1)
        return arg_result;
    return create_and_run_server(&config);
}
