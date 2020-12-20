#ifndef DYNARR_H_
#define DYNARR_H_

#include <stddef.h>


typedef struct
{
    size_t size;
    size_t alloc_size;
    void** data;
} dynarr_t;


#define dynarr_at(TYPE, ARR, IDX) ((TYPE*)ARR->data[IDX])

dynarr_t* dynarr_init();

void dynarr_push(dynarr_t* arr, void* value);

void dynarr_free(dynarr_t* arr, void(*free_callback)(void*));

void dynarr_swap(dynarr_t* arr, size_t idxa, size_t idxb);

#endif
