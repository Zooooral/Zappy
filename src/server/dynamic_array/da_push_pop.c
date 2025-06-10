/*
** EPITECH PROJECT, 2024
** src/da_modify
** File description:
** functions for simple modifications of the da
** push to add and pop to remove ()
*/

#include <assert.h>
#include <malloc.h>
#include <string.h>
#include <stdbool.h>

#include "server/dynamic_array.h"

void *da_push(void *array, const void *data, size_t size)
{
    da_info_t *info = (da_info_t *)array - 1;
    size_t new_cappacity = info->cappacity;

    if (info->len + size >= new_cappacity) {
        while (info->len + size >= new_cappacity) {
            new_cappacity *= 1.5;
        }
        info->cappacity = new_cappacity;
        info = realloc(info, new_cappacity);
        assert(info != NULL);
        array = info + 1;
    }
    memcpy((unsigned char *)info + info->len, data, size);
    info->len += size;
    return array;
}

void *da_push_mmem(void *array, const void *data, size_t size)
{
    da_info_t *info = (da_info_t *)array - 1;

    if (info->len + size >= info->cappacity) {
        info->cappacity += size;
        info = realloc(info, info->cappacity);
        assert(info != NULL);
        array = info + 1;
    }
    memcpy((unsigned char *)info + info->len, data, size);
    info->len += size;
    return array;
}

void *da_acces(void *array, size_t conv_i)
{
    da_info_t *info = (da_info_t *)array - 1;

    conv_i += sizeof *info;
    assert(conv_i < info->len);
    return array;
}

void da_remove_at(void *array, size_t index, size_t size)
{
    da_info_t *info = (da_info_t *)array - 1;
    unsigned char *ptr = (unsigned char *)info + index;

    assert(index < info->len);
    memmove(ptr, ptr + size, info->len - index - size);
    info->len -= size;
}

void da_pop(void *array, size_t size)
{
    DA_INFO(array).len -= size;
}
