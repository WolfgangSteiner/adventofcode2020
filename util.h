#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

typedef struct
{
  size_t size;
  size_t alloc_size;
  int* values;
} int_array;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MImplies(a, b) assert(((a) && (b)) || !(a))
#define MUnused(A) (void)(A)
void int_array_delete(int_array* array);
int_array* int_array_new();
void int_array_free(int_array* array);
void int_array_push_front(int_array* array, int value);
void int_array_push_back(int_array* array, int value);
void int_array_sort(int_array* array);
int int_array_front(const int_array* array);
int int_array_back(const int_array* array);
int int_array_at(const int_array* array, int index);
int_array* int_array_read(char* fileName);
void int_array_print(const int_array* array);
const char* copy_word(char* dst, const char* line);

void remove_array_elements(
    void** array,
    size_t* size,
    bool(*should_delete_callback)(const void*, const void*),
    const void* user_data);


int chomp(char* str);

#endif
