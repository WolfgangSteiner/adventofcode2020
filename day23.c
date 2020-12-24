#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "intarr.h"


char* test_input = "389125467";


void setUp()
{
}


void tearDown()
{
}


intarr_t* parse_input(char* str)
{
    intarr_t* arr = intarr_init(strlen(str));
    for (size_t i = 0; i < strlen(str); ++i)
    {
        intarr_push(arr, (int)(str[i] - '0'));
    }

    return arr;
}


size_t index_for_value(intarr_t* arr, int value)
{
    for (size_t i = 0; i < arr->size; ++i)
    {
        if (*intarr_at(arr, i) == value) return i;
    }

    MInvalid();
    return 0;
}


size_t inc_index(size_t idx, intarr_t* arr, size_t increment)
{
    return (idx + increment) % arr->size;
}

size_t dec_index(size_t idx, intarr_t* arr)
{
    return (idx + arr->size - 1) % arr->size;
}

bool is_index_removed(intarr_t* arr, size_t move_idx, size_t idx)
{
    return (idx - move_idx + arr->size) % arr->size <= 2;
}

bool is_value_removed(intarr_t* arr, size_t move_idx, int value)
{
    size_t value_idx = index_for_value(arr, value);
    return (is_index_removed(arr, move_idx, value_idx));
}


int dec_value(int value, int max_value)
{
    return ((value - 1) + max_value - 1) % max_value + 1;
}


int compute_destination_value(intarr_t* arr, int selected_value, size_t move_idx)
{
    int max_value = intarr_max(arr);
    int dst_value = dec_value(selected_value, max_value);

    while (is_value_removed(arr, move_idx, dst_value))
    {
        dst_value = dec_value(dst_value, max_value);
    }

    return dst_value;
}


void print_pick_up(intarr_t* arr, size_t move_idx)
{
    printf("Pick up: ");
    for (size_t i = 0; i < 3; ++i)
    {
        if (i > 0) printf(", ");
        printf("%d", arr->data[move_idx]);
        move_idx = inc_index(move_idx, arr, 1);
    }
    printf("\n");
}

intarr_t* play_round(intarr_t* arr, int selected_value)
{
    intarr_t* new_arr = intarr_init(arr->size);
    size_t current_idx = index_for_value(arr, selected_value);
    size_t move_idx = inc_index(current_idx, arr, 1);
    int dst_value = compute_destination_value(arr, selected_value, move_idx);
    //print_pick_up(arr, move_idx);
    //printf("Destination: %d\n", dst_value);



    size_t src_idx = 0;
    size_t count = 0;

    while (count < arr->size)
    {
        while(is_index_removed(arr, move_idx, src_idx))
        {
           src_idx = inc_index(src_idx, arr, 1);
        }

        int val = *intarr_at(arr, src_idx);
        intarr_push(new_arr, val);
        count++;
        src_idx = inc_index(src_idx, arr, 1);
        if (val == dst_value)
        {
            size_t do_move_idx = move_idx;
            for (size_t i = 0; i < 3; i++)
            {
                val = *intarr_at(arr, do_move_idx);
                intarr_push(new_arr, val);
                do_move_idx = inc_index(do_move_idx, arr, 1);
            }
            count += 3;
        }
    }

    //assert(arr->size == new_arr->size);
    //intarr_print(new_arr);
    intarr_free(arr);
    return new_arr;
}


char* calc_result(intarr_t* arr)
{
    char* result = calloc(arr->size, 1);
    
    size_t current_idx = index_for_value(arr, 1);
    current_idx = inc_index(current_idx, arr, 1);

    for (size_t i = 0; i < arr->size - 1; i++)
    {
        result[i] = '0' + (char)arr->data[current_idx];
        current_idx = inc_index(current_idx, arr, 1);
    }

    return result;
}


intarr_t* play_game(intarr_t* arr, size_t num_rounds)
{
    int selected_value = *intarr_at(arr, 0);

    for (size_t i = 0; i < num_rounds; ++i)
    {
       //printf("Selected value: %d\n", selected_value);
       arr = play_round(arr, selected_value);
       size_t selected_idx = index_for_value(arr, selected_value);
       selected_idx = inc_index(selected_idx, arr, 1);
       selected_value = *intarr_at(arr, selected_idx);
    } 

    return arr;
}


char* play_game_part_one(char* input, size_t num_rounds)
{
    intarr_t* arr = parse_input(input);
    arr = play_game(arr, num_rounds);
    char* result = calc_result(arr);
    intarr_free(arr);
    return result;
}

void extend_game(intarr_t* arr)
{
    for (size_t i = arr->size + 1; i <= 1000000; i++)
    {
       intarr_push(arr, i);
    }
} 


size_t play_game_part_two(char* input, size_t num_rounds)
{
    intarr_t* arr = parse_input(input);
    extend_game(arr);
    arr = play_game(arr, num_rounds);
    size_t start_idx = index_for_value(arr, 1);
    size_t idx_a = inc_index(start_idx, arr, 1);
    size_t idx_b = inc_index(start_idx, arr, 2);
    size_t a = *intarr_at(arr, idx_a);
    size_t b = *intarr_at(arr, idx_b);
    size_t result = a * b;

    intarr_free(arr);
    return result;
}

void test_parse_input()
{
    intarr_t* arr = parse_input(test_input);
    TEST_ASSERT_EQUAL_STRING("3 8 9 1 2 5 4 6 7", intarr_to_string(arr));
    intarr_free(arr);
}


int do_test_destination_value(char* input, int selected_value, size_t move_idx)
{
    intarr_t* arr = parse_input(input);
    int result = compute_destination_value(arr, selected_value, move_idx);
    intarr_free(arr);
    return result;
}


bool do_test_is_value_removed(char* input, size_t move_idx, int value)
{
    intarr_t* arr = parse_input(input);
    bool result = is_value_removed(arr, move_idx, value);
    intarr_free(arr);
    return result;
}


void test_is_value_removed()
{
    TEST_ASSERT_FALSE(do_test_is_value_removed("389125467", 1, 3));
    TEST_ASSERT_TRUE(do_test_is_value_removed("389125467", 1, 8));
    TEST_ASSERT_TRUE(do_test_is_value_removed("389125467", 1, 9));
    TEST_ASSERT_TRUE(do_test_is_value_removed("389125467", 1, 1));
    TEST_ASSERT_FALSE(do_test_is_value_removed("389125467", 1, 2));

    TEST_ASSERT_TRUE(do_test_is_value_removed("389125467", 7, 6));
    TEST_ASSERT_TRUE(do_test_is_value_removed("389125467", 7, 7));
    TEST_ASSERT_TRUE(do_test_is_value_removed("389125467", 7, 3));
    TEST_ASSERT_FALSE(do_test_is_value_removed("389125467", 7, 8));
    TEST_ASSERT_FALSE(do_test_is_value_removed("389125467", 7, 4));

}

void test_compute_destination_value()
{
    TEST_ASSERT_EQUAL(2, do_test_destination_value("389125467", 3, 1));
    TEST_ASSERT_EQUAL(7, do_test_destination_value("328915467", 2, 2));
}


void test_play_game()
{
    char* result = play_game_part_one("389125467", 10);
    TEST_ASSERT_EQUAL_STRING("92658374", result);
    free(result);
    result = play_game_part_one("389125467", 100);
    TEST_ASSERT_EQUAL_STRING("67384529", result);
    free(result);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_input);
    RUN_TEST(test_compute_destination_value);
    RUN_TEST(test_is_value_removed);
    RUN_TEST(test_play_game);
    UNITY_END();

   char* result = play_game_part_two("327465189", 1000);
    printf("Result part 2: %s\n", result);
    free(result);

    return 0;
}

