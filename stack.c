#include "stack.h"
#include <stdlib.h>

stack_t* stack_init(size_t alloc_size)
{
    stack_t* stack = calloc(1, sizeof(stack_t));
    stack->alloc_size = alloc_size;
    stack->data = malloc(sizeof(size_t) * alloc_size);

    return stack;
}


void stack_free(stack_t* stack)
{
    free(stack->data);
    free(stack);
}


size_t stack_size(const stack_t* stack)
{
    return stack->size;
}


static void stack_grow(stack_t* stack)
{
    stack->alloc_size *= 2;
    stack->data = realloc(stack->data, sizeof(size_t) * stack->alloc_size);
}


void stack_push_func(stack_t* stack, size_t value)
{
    if (stack->size == stack->alloc_size)
    {
        stack_grow(stack);
    }

    stack->data[stack->size++] = value;
}


size_t stack_pop_func(stack_t* stack)
{
    size_t value = stack->data[stack->size - 1];
    stack->size--;
    return value;
}


size_t stack_front_func(stack_t* stack)
{
    return stack->data[stack->size - 1];
}



