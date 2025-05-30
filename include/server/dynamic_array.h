/*
** EPITECH PROJECT, 2024
** include/da
** File description:
** header for dynamic array library da
*/

#ifndef DYNAMIC_ARRAY_LIB_H_
    #define DYNAMIC_ARRAY_LIB_H_
    #include <stddef.h>

    #define ARRAY_SCALING_FACTOR 1.5

typedef struct {
    size_t len;
    size_t cappacity;
} da_info_t;

//call these function to create and destroy dynamic arrays
void *da_create(void);
void *da_create_with_cappacity(size_t cappacity);
void da_destroy(void *darray);

    #define DA_INFO(ar) (((da_info_t *)(ar))[-1])
    #define DAC(ar, i) (ar = da_acces(ar, sizeof(*ar) * i)[i])
    #define DA_LEN(ar) ((DA_INFO(ar).len - sizeof(da_info_t)) / sizeof(*ar))
    #define DA_LAST(ar) ((ar)[DA_LEN(ar) - 1])

    // uses the da_push function to push an element
    #define DA_PUSH(ar, d) (ar = da_push(ar, &d, sizeof(d)))
    // uses the da_push function to push a pointer to an element
    #define DA_PUSH_PTR(ar, data) (ar = da_push(ar, data, sizeof(*data)))

    #define DA_POP(ar) (da_pop(ar, sizeof(*ar)))
    #define DA_POP_AT(ar, i) (da_remove_at(ar, i, sizeof(*ar)))

    #ifndef REF_FUNC_CALL
        #define REF_FUNC_CALL(func) &((typeof(func)[]) { (func) })[0]
    #endif

    // hacky way to push from a function call directly
    #define DA_PUSH_FUNC(ar, f) (ar = da_push(ar, REF_FUNC_CALL(f), sizeof(f)))

//used by macros
void *da_acces(void *array, size_t conv_i);

//function for simple utilities

// pushes an element of given size at the end
// O(1) and 1 in n times O(n) which means technically O(1)
void *da_push(void *array, const void *data, size_t size);

// O(n) does not do any fancy bigger reallocation than needed
void *da_push_mmem(void *array, const void *data, size_t size);

// very fast, (just decrements the size does not free memory ever)
// the last element is alive until an other push happens
void da_pop(void *array, size_t size);

// removes an element at the given index
void da_remove_at(void *array, size_t index, size_t size);
#endif
