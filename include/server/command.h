/*
** EPITECH PROJECT, 2025
** include/server/command
** File description:
** command utilities
*/

#ifndef ZAPPY_COMMANDS_H
    #define ZAPPY_COMMANDS_H

    #include <stdint.h>
    #include <stddef.h>

    #ifndef __BYTE_ORDER__
        #error "__BYTE_ORDER__ not defined by compiler."
    #endif

    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        #define IMPL_NCO(id_val32) (bswap_u32(id_val32))
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        #define IMPL_NCO(id_val32) (id_val32)
    #else
        #error "Unknown byte order defined by compiler."
    #endif

typedef void (*command_handler_t)(void *client, const char *command);

typedef enum {
    CCMD_MSZ,
    CCMD_BCT,
    CCMD_MCT,
    CCMD_TNA,
    CCMD_PPO_REQUEST,
    CCMD_PLV_REQUEST,
    CCMD_PIN_REQUEST,
    CCMD_SGT_REQUEST,
    CCMD_SST,
    CCMD_TYPE_COUNT,
    CCMD_TYPE_UNKNOWN
} ccmd_id_t;

static const uint32_t g_ccmd_lookup_table[CCMD_TYPE_COUNT] = {
    [CCMD_MSZ] = 'msz\n',
    [CCMD_BCT] = 'bct ',
    [CCMD_MCT] = 'mct\n',
    [CCMD_TNA] = 'tna\n',
    [CCMD_PPO_REQUEST] = 'ppo ',
    [CCMD_PLV_REQUEST] = 'plv ',
    [CCMD_PIN_REQUEST] = 'pin ',
    [CCMD_SGT_REQUEST] = 'sgt\n',
    [CCMD_SST] = 'sst '
};

typedef enum {
    SCMD_MAP_SIZE,
    SCMD_TILE_CONTENT,
    SCMD_MAP_CONTENT,
    SCMD_TEAM_NAMES,
    SCMD_NEW_PLAYER_CONNECTION,
    SCMD_PLAYER_POSITION,
    SCMD_PLAYER_LEVEL,
    SCMD_PLAYER_INVENTORY,
    SCMD_PLAYER_EXPULSION,
    SCMD_PLAYER_BROADCAST,
    SCMD_INCANTATION_START,
    SCMD_INCANTATION_END,
    SCMD_PLAYER_EGG_LAYING,
    SCMD_PLAYER_RESOURCE_DROP,
    SCMD_PLAYER_RESOURCE_COLLECT,
    SCMD_PLAYER_DEATH,
    SCMD_EGG_LAID_BY_PLAYER,
    SCMD_EGG_HATCHING,
    SCMD_EGG_PLAYER_CONNECTION,
    SCMD_EGG_DEATH,
    SCMD_TIME_UNIT_RESPONSE,
    SCMD_TIME_UNIT_MODIFIED,
    SCMD_GAME_END,
    SCMD_SERVER_MESSAGE,
    SCMD_SERVER_UNKNOWN_COMMAND,
    SCMD_SERVER_BAD_PARAMETER,
    SCMD_TYPE_COUNT,
    SCMD_TYPE_UNKNOWN
} scmd_id_t;

static const uint32_t g_scmd_lookup_table[SCMD_TYPE_COUNT] = {
    [SCMD_MAP_SIZE] = 'msz ',
    [SCMD_TILE_CONTENT] = 'bct ',
    [SCMD_MAP_CONTENT] = 'mct ',
    [SCMD_TEAM_NAMES] = 'tna ',
    [SCMD_NEW_PLAYER_CONNECTION] = 'pnw ',
    [SCMD_PLAYER_POSITION] = 'ppo ',
    [SCMD_PLAYER_LEVEL] = 'plv ',
    [SCMD_PLAYER_INVENTORY] = 'pin ',
    [SCMD_PLAYER_EXPULSION] = 'pex ',
    [SCMD_PLAYER_BROADCAST] = 'pbc ',
    [SCMD_INCANTATION_START] = 'pic ',
    [SCMD_INCANTATION_END] = 'pie ',
    [SCMD_PLAYER_EGG_LAYING] = 'pfk ',
    [SCMD_PLAYER_RESOURCE_DROP] = 'pdr ',
    [SCMD_PLAYER_RESOURCE_COLLECT] = 'pgt ',
    [SCMD_PLAYER_DEATH] = 'pdi ',
    [SCMD_EGG_LAID_BY_PLAYER] = 'enw ',
    [SCMD_EGG_HATCHING] = 'eht ',
    [SCMD_EGG_PLAYER_CONNECTION] = 'ebo ',
    [SCMD_EGG_DEATH] = 'edi ',
    [SCMD_TIME_UNIT_RESPONSE] = 'sgt ',
    [SCMD_TIME_UNIT_MODIFIED] = 'sst ',
    [SCMD_GAME_END] = 'seg ',
    [SCMD_SERVER_MESSAGE] = 'smg ',
    [SCMD_SERVER_UNKNOWN_COMMAND] = 'suc\n',
    [SCMD_SERVER_BAD_PARAMETER] = 'sbp\n'
};

void chandler_mapsize(void *, char const *cmd);
void chandler_tilecontent(void *, char const *cmd);
void chandler_mapcontent(void *, char const *cmd);
void chandler_teamnames(void *, char const *cmd);
void chandler_player_position(void *, char const *cmd);
void chandler_player_level(void *, char const *cmd);
void chandler_player_inventory(void *, char const *cmd);
void chandler_player_expelled(void *, char const *cmd);
void chandler_time_unit(void *, char const *cmd);
void chandler_time_unit_modification(void *, char const *cmd);

static const command_handler_t g_ccmd_handlers[] = {
    [CCMD_MSZ] = chandler_mapsize,
    [CCMD_BCT] = chandler_tilecontent,
    [CCMD_MCT] = chandler_mapcontent,
    [CCMD_TNA] = chandler_teamnames,
    [CCMD_PPO_REQUEST] = chandler_player_position,
    [CCMD_PLV_REQUEST] = chandler_player_level,
    [CCMD_PIN_REQUEST] = chandler_player_inventory,
    [CCMD_SGT_REQUEST] = chandler_time_unit,
    [CCMD_SST] = chandler_time_unit_modification
};

static inline uint32_t bswap_u32(uint32_t val)
{
    return ((val & 0xFF000000U) >> 24) |
        ((val & 0x00FF0000U) >> 8) |
        ((val & 0x0000FF00U) << 8) |
        ((val & 0x000000FFU) << 24);
}

static inline uint32_t client_command_by_id(ccmd_id_t cmd_type)
{
    uint32_t cmd_id_32;

    if (cmd_type >= CCMD_TYPE_COUNT)
        return (uint32_t)-1;
    cmd_id_32 = g_ccmd_lookup_table[cmd_type];
    return IMPL_NCO(cmd_id_32);
}

static inline ccmd_id_t client_command_id_from_string(const char *str_command)
{
    if (str_command == NULL || str_command[0] == '\0' ||
        str_command[1] == '\0' || str_command[2] == '\0' ||
        str_command[3] == '\0')
        return CCMD_TYPE_UNKNOWN;
    for (ccmd_id_t i = 0; i < CCMD_TYPE_COUNT; ++i)
        if (g_ccmd_lookup_table[i] == *((uint32_t *)str_command))
            return i;
    return CCMD_TYPE_UNKNOWN;
}

static inline uint32_t server_command_by_id(scmd_id_t cmd_type)
{
    uint32_t cmd_id_32;

    if (cmd_type >= SCMD_TYPE_COUNT)
        return -1;
    cmd_id_32 = g_scmd_lookup_table[cmd_type];
    return IMPL_NCO(cmd_id_32);
}

static inline scmd_id_t server_command_id_from_string(const char *str_command)
{
    if (str_command == NULL || str_command[0] == '\0' ||
        str_command[1] == '\0' || str_command[2] == '\0' ||
        str_command[3] == '\0')
        return SCMD_TYPE_UNKNOWN;
    for (scmd_id_t i = 0; i < SCMD_TYPE_COUNT; ++i)
        if (g_scmd_lookup_table[i] == *((uint32_t *)str_command))
            return i;
    return SCMD_TYPE_UNKNOWN;
}

#endif /* ZAPPY_COMMANDS_H */
