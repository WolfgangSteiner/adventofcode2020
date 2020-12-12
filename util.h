#ifndef UTIL_H_
#define UTIL_H_

#include <stdlib.h>
#include <assert.h>

typedef struct
{
  size_t size;
  size_t alloc_size;
  int* values;
} int_array;

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MImplies(a, b) assert((a) && (b) || !(a))

void int_array_delete(int_array* array);
int_array* int_array_new();
void int_array_push_front(int_array* array, int value);
void int_array_push_back(int_array* array, int value);
void int_array_sort(int_array* array);
int int_array_front(int_array* array);
int int_array_back(int_array* array);
int_array* int_array_read(char* fileName);
void int_array_print(const int_array* array);
const char* copy_word(char* dst, const char* line);

#endif
