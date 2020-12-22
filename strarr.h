#ifndef STRARR_H_
#define STRARR_H_
#include "basetypes.h"

typedef struct
{
    size_t size;
    size_t alloc_size;
    char** data;
} strarr_t;


strarr_t* strarr_init();
strarr_t* strarr_init_with_size(size_t alloc_size);
void strarr_free(strarr_t* arr);
strarr_t* strarr_copy(strarr_t* arr);
char* strarr_alloc_str(strarr_t* strarr, size_t size);
void strarr_push(strarr_t* strarr, const char* str);
void strarr_clear(strarr_t* arr);
bool strarr_contains(const strarr_t* arr, const char* str);
char* strarr_at(strarr_t* arr, size_t idx);
void strarr_remove(strarr_t* arr, char* str);
void strarr_remove_any(strarr_t* arr, const strarr_t* of_arr);
strarr_t* strarr_intersect(strarr_t* a, strarr_t* b);
void strarr_sort(strarr_t* arr);
char* strarr_join(strarr_t* arr, char* join_str);
char* strarr_copy_string_at(strarr_t* arr, size_t idx);
#endif
