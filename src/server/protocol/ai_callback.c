/*
** EPITECH PROJECT, 2025
** /home/vj/coding/B-YEP-400-PAR-4-1-zappy-maxence.bunel/src/server/protocol/ai_callback
** File description:
** ai callbacks (after the ticks passed)
*/

#include "server/dynamic_array.h"
#include "server/server.h"
#include "server/protocol_ai.h"
#include "server/server_updates.h"
#include "server/ai_command_handlers.h"

static const void (*action_handlers[])(server_t *, client_t *, char *cmd) = {

    [AI_ACTION_FORWARD] = ai_action_forward,
    [AI_ACTION_RIGHT] = ai_action_right,
    [AI_ACTION_LEFT] = ai_action_left,
    [AI_ACTION_LOOK] = ai_action_look,
    [AI_ACTION_INVENTORY] = ai_action_inventory,
    [AI_ACTION_TAKE] = ai_action_take,
    [AI_ACTION_SET] = ai_action_set,
    [AI_ACTION_INCANTATION] = ai_action_incantation,
    [AI_ACTION_EJECT] = ai_action_eject,
    [AI_ACTION_FORK] = ai_action_fork,
    [AI_ACTION_CONNECT_NBR] = ai_action_connect_nbr,
    [AI_ACTION_BROADCAST] = ai_action_broadcast,
};

void ai_callback_handler(client_t *client, void *data)
{
    ai_action_data_t *action_data = (ai_action_data_t *)data;
    server_t *server;

    if (!action_data || !client)
        return;
    server = action_data->server;
    if (action_data->type >= 0 && action_data->type <= sizeof action_handlers &&
        action_handlers[action_data->type]) {
        action_handlers[action_data->type](server, client, action_data->cmd);
    }
    free(action_data);
}
