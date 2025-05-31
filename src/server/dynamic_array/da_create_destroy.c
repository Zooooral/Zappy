/*
** EPITECH PROJECT, 2024
** src/da_create_destroy
** File description:
** header file for dynamic array creating and destuction
*/

#include <assert.h>
#include <malloc.h>
#include "server/dynamic_array.h"

void *da_create_with_cappacity(size_t size)
{
    da_info_t *new_dar = malloc(size + sizeof(da_info_t));
    da_info_t data = {0};

    assert(new_dar != NULL);
    data.cappacity = size + sizeof data;
    data.len = sizeof data;
    new_dar[0] = data;
    return new_dar + 1;
}

void *da_create(void)
{
    return da_create_with_cappacity(sizeof(da_info_t));
}

void da_destroy(void *darray)
{
    da_info_t *dar = (da_info_t *)darray - 1;

    if (!darray)
        return;
    free(dar);
}
