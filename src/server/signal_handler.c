/*
** EPITECH PROJECT, 2025
** src/server/signal_handler.c
** File description:
** Enhanced signal handling for graceful shutdown
*/

#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>

int signal_handler_init(void)
{
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGQUIT);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
        perror("sigprocmask");
        return -1;
    }
    printf("[SERVER] Signal handler initialized\n");
    return 0;
}

void signal_handler_cleanup(int signal_fd)
{
    (void)signal_fd;
    printf("[SERVER] Signal handler cleaned up\n");
}

int signal_handler_check(int signal_fd)
{
    sigset_t mask;
    struct timespec timeout = {0, 0};
    int sig;

    (void)signal_fd;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGQUIT);
    sig = sigtimedwait(&mask, NULL, &timeout);
    if (sig > 0) {
        printf("[SERVER] Received shutdown signal: %d\n", sig);
        return 1;
    }
    return 0;
}
