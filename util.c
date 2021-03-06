#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INT_ARRAY_INITIAL_ALLOC_SIZE 16
#define INT_ARRAY_REALLOC_NUMERATOR 8
#define INT_ARRAY_REALLOC_DENOMINATOR 5


void int_array_delete(int_array* array)
{
  free(array->values);
  free(array);
}


int_array* int_array_new()
{
    int_array* array = malloc(sizeof(int_array));
    array->size = 0;
    array->alloc_size = INT_ARRAY_INITIAL_ALLOC_SIZE;
    array->values = malloc(sizeof(int) * INT_ARRAY_INITIAL_ALLOC_SIZE);
    return array;
}


void int_array_free(int_array* array)
{
    free(array->values);
    free(array);
}


void int_array_push_front(int_array* array, int value)
{
    if (array->size == array->alloc_size)
    {
        const size_t newAllocSize = array->alloc_size * INT_ARRAY_REALLOC_NUMERATOR / INT_ARRAY_REALLOC_DENOMINATOR;
        array->values = realloc(array->values, newAllocSize * sizeof(int));
        array->alloc_size = newAllocSize;
    }

    memmove(array->values + 1, array->values, array->size * sizeof(int));
    array->values[0] = value;
    array->size++;
}


void int_array_push_back(int_array* array, int value)
{
    if (array->size == array->alloc_size)
    {
        const size_t newAllocSize = array->alloc_size * INT_ARRAY_REALLOC_NUMERATOR / INT_ARRAY_REALLOC_DENOMINATOR;
        array->values = realloc(array->values, newAllocSize * sizeof(int));
        array->alloc_size = newAllocSize;
    }

    array->values[array->size] = value;
    array->size++;
}

int int_array_compare(const void* pa, const void* pb)
{
    const int a = *(int*)pa;
    const int b = *(int*)pb;
    return a < b ? -1 : a == b ? 0 : 1;
}

void int_array_sort(int_array* array)
{
    qsort(array->values, array->size, sizeof(int), int_array_compare);
}


int int_array_front(const int_array* array)
{
    assert(array->size);
    return array->values[0];
}


int int_array_back(const int_array* array)
{
    assert(array->size);
    return array->values[array->size - 1];
}

int int_array_at(const int_array* array, size_t index)
{
    assert(array->size);
    assert(index < array->size);
    return array->values[index];
}

int_array* int_array_read(char* fileName)
{
    FILE* fp = fopen(fileName, "r");
    int_array* array = int_array_new();

    int value;
    while (fscanf(fp, "%d", &value) != EOF)
    {
        int_array_push_back(array, value);
    }

    return array;
}


void int_array_print(const int_array* array)
{
    for (size_t i = 0; i < array->size; ++i)
    {
        printf("%d\n", array->values[i]);
    }
}


const char* copy_word(char* dst, const char* line)
{
    const char* iter = line;
 
    while (isalnum(*iter) || *iter == '+' || *iter == '-')
    {
        iter++;
    }
    const size_t wordLength = iter - line;
    strncpy(dst, line, wordLength);
    dst[wordLength] = 0; 

    while (!(isalnum(*iter) || *iter == '+' || *iter == '-') && *iter != '\0')
    {
        iter++;
    }

    if (*iter == '\0')
    {
        return 0;
    }
    else
    {
        return iter;
    }
}

int chomp(char* str)
{
    int length = strlen(str);
    while (length && str[length - 1] == '\n')
    {
        str[length - 1] = '\0';
        length--;
    }

    return length;
}

void remove_array_elements(
    void** array,
    size_t* size,
    bool(*should_delete_callback)(const void*, const void*),
    const void* user_data,
    void(*deallocator)(void*))
{
    size_t start_idx = 0;
    size_t end_idx = start_idx;

    while (true)
    {
        while (!should_delete_callback(array[start_idx], user_data))
        {
            start_idx++;
            if (start_idx == *size)
            {
                return;
            } 
        }

        end_idx = start_idx;

        while (should_delete_callback(array[end_idx], user_data))
        {
            if (deallocator)
            {
                deallocator(array[end_idx]);
            }

            end_idx++;
            if (end_idx == *size)
            {
                *size = start_idx;
                return;        
            }
        }

        size_t num_elements_to_move = *size - end_idx;
        size_t num_elements_to_delete = end_idx - start_idx;

        memmove(&array[start_idx], &array[end_idx], num_elements_to_move * sizeof(void*));
        *size -= num_elements_to_delete;
    }
}

void swap_u32(u32* pa, u32* pb)
{
    u32 c = *pa;
    *pa = *pb;
    *pb = c;
}

void swap_bool(bool* pa, bool* pb)
{
    bool c = *pa;
    *pa = *pb;
    *pb = c;
}

void swap_u8(u8* pa, u8* pb)
{
    u8 c = *pa;
    *pa = *pb;
    *pb = c;
}

