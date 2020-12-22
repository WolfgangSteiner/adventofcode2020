#include "strarr.h"
#include <stdlib.h>
#include <string.h>
#include "util.h"

strarr_t* strarr_init()
{
    return strarr_init_with_size(16);
}

strarr_t* strarr_init_with_size(size_t alloc_size)
{
    strarr_t* strarr = calloc(1, sizeof(strarr_t));
    strarr->alloc_size = alloc_size;
    strarr->data = malloc(sizeof(char*) * strarr->alloc_size);
    return strarr;
}

void strarr_free(strarr_t* arr)
{
    for (size_t i = 0; i < arr->size; i++)
    {
        free(arr->data[i]);
    }
    free(arr->data);
    free(arr);
}


strarr_t* strarr_copy(strarr_t* arr)
{
    strarr_t* copy = strarr_init_with_size(arr->size);
    for (size_t i = 0; i < arr->size; ++i)
    {
        strarr_push(copy, arr->data[i]);
    }

    return copy; 
}


char* strarr_alloc_str(strarr_t* strarr, size_t size)
{
    if (strarr->size == strarr->alloc_size)
    {
        strarr->alloc_size *= 2;
        strarr->data = realloc(strarr->data, sizeof(char*) * strarr->alloc_size);
    }

    char** bucket = &strarr->data[strarr->size++];
    *bucket = malloc(size);
    return *bucket;
}


void strarr_push(strarr_t* strarr, const char* str)
{
    char* bucket = strarr_alloc_str(strarr, strlen(str) + 1);
    strcpy(bucket, str);
}


void strarr_clear(strarr_t* arr)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        free(arr->data[i]);
    }

    arr->size = 0;
}


bool strarr_contains(const strarr_t* arr, const char* str)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        if (!strcmp(arr->data[i], str))
        {
            return true;
        }
    }

    return false;
}


static bool delete_string_callback(const void* element, const void* user_data)
{
    return !strcmp((char*)element, (char*) user_data);
}


void strarr_remove(strarr_t* arr, char* str)
{
    remove_array_elements((void**)arr->data, &arr->size, delete_string_callback, str, free);
} 


char* strarr_at(strarr_t* arr, size_t idx)
{
    assert(idx < arr->size);
    return arr->data[idx];
}

strarr_t* strarr_intersect(strarr_t* a, strarr_t* b)
{
    strarr_t* result = strarr_init();

    for (size_t i = 0; i < a->size; i++)
    {
        char* str = a->data[i];
        if (strarr_contains(b, str))
        {
            strarr_push(result, str);
        }
    }

    return result;
}


static int strarr_sort_compare(const void* pa, const void* pb)
{
    const char* const * str_a_ptr = pa;
    const char* const * str_b_ptr = pb;
    return strcmp(*str_a_ptr, *str_b_ptr);
}



void strarr_sort(strarr_t* arr)
{
    qsort(arr->data, arr->size, sizeof(char*), strarr_sort_compare);
}


static size_t strarr_total_size(strarr_t* arr)
{
    size_t result = 0;
    for (size_t i = 0; i < arr->size; i++)
    {
        result += strlen(strarr_at(arr, i));
    }

    return result;
}


char* strarr_join(strarr_t* arr, char* join_str)
{
    if (arr->size == 0)
    {
        return calloc(1, sizeof(char));
    }

    size_t alloc_size = strarr_total_size(arr) + (arr->size - 1) * strlen(join_str) + 1; 
    char* result = calloc(alloc_size, sizeof(char));
    char* pos = result;
    size_t join_str_len = strlen(join_str);

    for (size_t i = 0; i < arr->size; ++i)
    {
        char* str = strarr_at(arr, i);
        strcpy(pos, str);
        pos += strlen(str);

        if (i != arr->size - 1)
        {
            strcpy(pos, join_str);
            pos += join_str_len; 
        }
    }

    return result;
}


static bool delete_any_string_callback(const void* element, const void* user_data)
{
    const char* str = element;
    const strarr_t* of_arr = user_data;

    return strarr_contains(of_arr, str);
}


void strarr_remove_any(strarr_t* arr, const strarr_t* of_arr)
{
    remove_array_elements((void**)arr->data, &arr->size, delete_any_string_callback, of_arr, free);
}


char* strarr_copy_string_at(strarr_t* arr, size_t idx)
{
    assert(idx < arr->size);
    const char* str = arr->data[idx];
    char* copy = malloc(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
}
