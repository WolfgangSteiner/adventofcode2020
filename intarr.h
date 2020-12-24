#ifndef INTARR_H_
#define INTARR_H_

#include "basetypes.h"

typedef struct
{
    size_t size;
    size_t alloc_size;
    int* data;
}intarr_t;


intarr_t* intarr_init(size_t initial_alloc_size);
void intarr_free(intarr_t* arr);
intarr_t* intarr_copy(intarr_t* arr);

void intarr_push(intarr_t* arr, int value);
int intarr_pop(intarr_t* arr);
void intarr_clear(intarr_t* arr);
int* intarr_at(intarr_t* arr, size_t index);
void intarr_print(intarr_t* arr);
bool intarr_contains(intarr_t* arr, int value);
char* intarr_to_string(const intarr_t* arr);

int intarr_max(const intarr_t* arr);

#endif
