/*
** EPITECH PROJECT, 2025
** include/server/win_condition.h
** File description:
** Game win condition header for Zappy
*/

#ifndef WIN_CONDITION_H
    #define WIN_CONDITION_H

    #include "server/server.h"

// Check if any team has won the game, returns team ID or -1 if no winner
int check_win_condition(server_t *server);

// Handle the win condition when a team wins
void handle_game_win(server_t *server, int winning_team_id);

#endif // WIN_CONDITION_H
