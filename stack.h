#ifndef STACK_H_
#define STACK_H_

#include <stddef.h>
#include <assert.h>

typedef struct
{
    size_t size;
    size_t alloc_size;
    size_t* data;
} stack_t;


#define stack_push(STACK, VALUE) stack_push_func(STACK, (size_t)VALUE)
#define stack_pop(TYPE, STACK) (TYPE)stack_pop_func(STACK)
#define stack_front(TYPE, STACK) (TYPE)stack_front_func(STACK)

stack_t* stack_init(size_t alloc_size);
void stack_free(stack_t* stack);
size_t stack_size(const stack_t* stack);
void stack_push_func(stack_t* stack, size_t element);
size_t stack_pop_func(stack_t* stack);
size_t stack_front_func(stack_t* stack);

#endif
