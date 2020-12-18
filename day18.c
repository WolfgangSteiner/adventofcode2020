#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include <stdint.h>
#include <ctype.h>
#include "stack.h"

typedef uint64_t u64;
size_t add = (size_t) -1l;
size_t mult = (size_t) -2l;
size_t brace = (size_t) -3l;


void setUp()
{
}


void tearDown()
{
}

void perform_operation(stack_t* stack, u64 value)
{
    size_t operator = stack_pop(size_t, stack);
    assert(operator == add || operator == mult);
    const u64 operand = stack_pop(u64, stack);
    const u64 result = operator == add ? value + operand : value * operand;
    stack_push(stack, result);
}

void perform_brace_close(stack_t* stack)
{
    u64 current_value = stack_pop(u64, stack);

    assert(stack_front(u64, stack) == brace);
    stack_pop(u64, stack);
    if (stack_size(stack) == 0u || stack_front(u64, stack) == brace)
    {
        stack_push(stack, current_value);
    }
    else 
    {
        perform_operation(stack, current_value);
    }
}

void perform_brace_close_part_two(stack_t* stack)
{
    u64 current_value = stack_pop(u64, stack);

    while (stack_front(u64, stack) == mult)
    {
        perform_operation(stack, current_value);
        current_value = stack_pop(u64, stack);
    }

    assert(stack_front(u64, stack) == brace);
    stack_pop(u64, stack);

    if (stack_front(u64, stack) == add)
    {
        perform_operation(stack, current_value);
    }
    else
    {
        stack_push(stack, current_value);

    }
}


void print_stack(stack_t* stack)
{
    for (size_t i = 0; i < stack->size; ++i)
    {
        u64 value = stack->data[i];
        if (value == brace)
        {
            printf(" ( ");
        }
        else if (value == add)
        {
            printf(" + ");
        }
        else if (value == mult)
        {
            printf(" * ");
        }
        else
        {
            printf(" %lu ", value);
        }
    }
    printf("\n");
}

u64 evaluate_expression(const char* expression, stack_t* stack)
{
    const char* pos = expression;

    while(*pos != 0)
    {
        const char c = *pos;
        if (isdigit(c))
        {
            const u64 value = atoi(pos);

            if (stack_size(stack) == 0 || stack_front(u64, stack) == brace)
            {
                stack_push(stack, value);
            }
            else
            {
                perform_operation(stack, value);
            }
        }
        else if (c == '+')
        {
            stack_push(stack, add);
        }
        else if (c == '*')
        {
            stack_push(stack, mult);
        }
        else if (c == '(')
        {
            stack_push(stack, brace);
        }
        else if (c == ')')
        {
            perform_brace_close(stack);
        }

        pos++;
    }

    assert(stack_size(stack) == 1u);
    return stack_pop(u64, stack); 
}


u64 evaluate_expression_part_two(const char* expression, stack_t* stack)
{
    const char* pos = expression;
  
    #if 0
    printf("==============================================\n");
    printf("%s\n", expression);
    printf("==============================================\n");
    #endif
    
    while(*pos != 0)
    {
        const char c = *pos;
        if (c == ' ') 
        {
            pos ++;
            continue;
        }
        // printf("Token: %c\n", c);
        if (isdigit(c))
        {
            const u64 value = atoi(pos);

            if (stack_front(u64, stack) == add)
            {
                perform_operation(stack, value);
            }
            else 
            {
                stack_push(stack, value);
            }

        }
        else if (c == '+')
        {
            stack_push(stack, add);
        }
        else if (c == '*')
        {
            stack_push(stack, mult);
        }
        else if (c == '(')
        {
            stack_push(stack, brace);
        }
        else if (c == ')')
        {
            perform_brace_close_part_two(stack);
        }

        //print_stack(stack);
        pos++;
    }

    while (stack_size(stack) > 1)
    {
        u64 value = stack_pop(u64, stack);
        assert(stack_front(u64, stack) == mult);
        perform_operation(stack, value);
    }

    assert(stack_size(stack) == 1u);
    return stack_pop(u64, stack); 
}


u64 evaluate_file(const char* file_name, u64(*evaluate_callback)(const char*, stack_t*))
{
    u64 result = 0u;
    stack_t* stack = stack_init(64);
    FILE* fp = fopen(file_name, "r");
    char line[1024];

    while(!feof(fp))
    {
        if (fgets(line, 1024, fp))
        {
            chomp(line);
            result += evaluate_callback(line, stack);
        }
    }

    fclose(fp);
    stack_free(stack);

    return result;
}


void test_expressions()
{
    stack_t* stack = stack_init(128);
    TEST_ASSERT_EQUAL(71, evaluate_expression("1 + 2 * 3 + 4 * 5 + 6", stack));
    TEST_ASSERT_EQUAL(51, evaluate_expression("1 + (2 * 3) + (4 * (5 + 6))", stack));
    TEST_ASSERT_EQUAL(26, evaluate_expression("2 * 3 + (4 * 5)", stack));
    TEST_ASSERT_EQUAL(437, evaluate_expression("5 + (8 * 3 + 9 + 3 * 4 * 3)", stack));
    TEST_ASSERT_EQUAL(12240, evaluate_expression("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))", stack));
    TEST_ASSERT_EQUAL(13632, evaluate_expression("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2", stack));
    TEST_ASSERT_EQUAL(231, evaluate_expression("(1 + 2) * (3 + 4) * (5 + 6)", stack));
    stack_free(stack);
}

void test_expressions_part_two()
{
    stack_t* stack = stack_init(128);
    TEST_ASSERT_EQUAL(231, evaluate_expression_part_two("1 + 2 * 3 + 4 * 5 + 6", stack));
    TEST_ASSERT_EQUAL(51, evaluate_expression_part_two("1 + (2 * 3) + (4 * (5 + 6))", stack));
    TEST_ASSERT_EQUAL(46, evaluate_expression_part_two("2 * 3 + (4 * 5)", stack));
    TEST_ASSERT_EQUAL(1445, evaluate_expression_part_two("5 + (8 * 3 + 9 + 3 * 4 * 3)", stack));
    TEST_ASSERT_EQUAL(669060, evaluate_expression_part_two("5 * 9 * (7 * 3 * 3 + 9 * 3 + (8 + 6 * 4))", stack));
    TEST_ASSERT_EQUAL(23340, evaluate_expression_part_two("((2 + 4 * 9) * (6 + 9 * 8 + 6) + 6) + 2 + 4 * 2", stack));
    stack_free(stack);
}



int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_expressions);
    RUN_TEST(test_expressions_part_two);
    UNITY_END();

    u64 result_part_one = evaluate_file("day18.txt", evaluate_expression);
    printf("Result Part 1: %lu\n", result_part_one);

    u64 result_part_two = evaluate_file("day18.txt", evaluate_expression_part_two);
    printf("Result Part 2: %lu\n", result_part_two);

    return 0;
}

