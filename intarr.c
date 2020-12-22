#include "intarr.h"
#include "stdlib.h"
#include <assert.h>
#include "stdio.h"
#include "string.h"

intarr_t* intarr_init(size_t initial_alloc_size)
{
    intarr_t* arr = calloc(1, sizeof(intarr_t));
    arr->alloc_size = initial_alloc_size;
    arr->data = malloc(initial_alloc_size * sizeof(int));
    return arr;
}    

void intarr_free(intarr_t* arr)
{
    free(arr->data);
    free(arr);
}

intarr_t* intarr_copy(intarr_t* arr)
{
    intarr_t* copy = intarr_init(arr->size);
    copy->size = arr->size;
    memcpy(copy->data, arr->data, sizeof(int) * arr->size);
    return copy;
}


void intarr_push(intarr_t* arr, int value)
{
    if (arr->size == arr->alloc_size)
    {
        arr->alloc_size *= 2;
        arr->data = realloc(arr->data, arr->alloc_size * sizeof(int));
    }

    arr->data[arr->size++] = value;
}

int intarr_pop(intarr_t* arr)
{
    int result = arr->data[0];
    memmove(arr->data, arr->data + 1, sizeof(int) * (arr->size - 1));
    arr->size--;
    return result;
}

void intarr_clear(intarr_t* arr)
{
    arr->size = 0;
}


int* intarr_at(intarr_t* arr, size_t index)
{
    assert(index < arr->size);
    return &arr->data[index];
}


void intarr_print(intarr_t* arr)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        printf("%d ", *intarr_at(arr, i));
    }

    printf("\n");
}


bool intarr_contains(intarr_t* arr, int value)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        if (*intarr_at(arr, i) == value) return true;
    }

    return false;
}    


char* intarr_to_string(const intarr_t* arr)
{
    size_t result_size = 256;
    char* result = malloc(result_size);
    char buffer[128];
    size_t pos = 0;

    for (size_t i = 0; i < arr->size; ++i)
    {
        int value = *intarr_at((intarr_t*)arr, i);
        snprintf(buffer, 128, "%d", value);
        size_t value_length = strlen(buffer);

        if (pos + value_length + 2 > result_size)
        {
            result_size *= 2;
            result = realloc(result, result_size);
        }

        if (i > 0)
        {
            result[pos] = ' ';
            pos++;
            result[pos] = 0;
        }
        strcpy(result + pos, buffer);
        pos += value_length;
    }
    
    return result;
}


