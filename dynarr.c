#include "dynarr.h"
#include <stdlib.h>
#include <assert.h>

dynarr_t* dynarr_init()
{
    dynarr_t* arr = calloc(1, sizeof(dynarr_t));
    arr->alloc_size = 16;
    arr->data = calloc(arr->alloc_size, sizeof(void*));
    return arr;
}

void dynarr_push(dynarr_t* arr, void* value)
{
    if (arr->size == arr->alloc_size)
    {
        arr->alloc_size *= 2;
        arr->data = reallocarray(arr->data, arr->alloc_size, sizeof(void*));
        assert(arr->data);
    }

    arr->data[arr->size++] = value;
}

void dynarr_free(dynarr_t* arr, void(*free_callback)(void*))
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        free_callback(arr->data[i]);
    }
    free(arr->data);
    free(arr);
}

void dynarr_swap(dynarr_t* arr, size_t idxa, size_t idxb)
{
    void* swap;
    swap = arr->data[idxa];
    arr->data[idxa] = arr->data[idxb];
    arr->data[idxb] = swap;
}

