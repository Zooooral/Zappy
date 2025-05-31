/*
** EPITECH PROJECT, 2025
** include/server/signal_handler.h
** File description:
** Signal handling functions
*/

#ifndef SIGNAL_HANDLER_H_
    #define SIGNAL_HANDLER_H_

int signal_handler_init(void);
void signal_handler_cleanup(int signal_fd);
int signal_handler_check(int signal_fd);

#endif /* !SIGNAL_HANDLER_H_ */
