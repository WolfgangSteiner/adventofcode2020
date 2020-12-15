#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "hash_map_int.h"

void setUp()
{
}


void tearDown()
{
}


typedef struct
{
    int last_number;
    int round_number;
    HashMap_int* map;
} GameState;

typedef struct
{
    int last_round;
    int previous_round;
} GameNumber;


void game_state_insert(GameState* state, int number)
{
    HashMapIterator_int* find_iter = hash_map_find_int(state->map, number);
    GameNumber* game_number = NULL;

    state->round_number++;
    state->last_number = number;

    if (hash_map_iterator_is_end_int(find_iter))
    {
        // printf("Inserting new number %d\n", number);
        game_number = calloc(1, sizeof(GameNumber));
        hash_map_insert_int(state->map, number, game_number);
        game_number->previous_round = state->round_number;
        game_number->last_round = state->round_number;
    }
    else
    {
        // printf("Updating new number %d\n", number);
        game_number = calloc(1, sizeof(GameNumber));
        game_number = hash_map_iterator_get_value_int(find_iter);
        game_number->previous_round = game_number->last_round;
        game_number->last_round = state->round_number;
    }

    // printf("last_round: %d, previous_round: %d\n", game_number->last_round, game_number->previous_round);

    free(find_iter); 
}
    
void game_state_free(GameState* state)
{
    hash_map_free_int(state->map, free);
    free(state);
}


GameState* read_input(const char* file_name)
{
    GameState* state = calloc(1, sizeof(GameState));
    state->map = hash_map_init_int(65535 << 4);
    state->round_number = 0;

    char* line = malloc(128);
    char* pos = line;
    FILE* fp = fopen(file_name, "r");
    fgets(line, 128, fp);
    fclose(fp);

    chomp(line);
    int number = 0;

    while (true)
    {
        sscanf(pos, "%d", &number);
        game_state_insert(state, number);
        pos = strstr(pos, ",");
        if (pos == NULL) break; 
        pos++;
    }

    free(line);

    return state;
}


int play_game_round(GameState* state)
{
    HashMapIterator_int* iter = hash_map_find_int(state->map, state->last_number);
    GameNumber* game_number = hash_map_iterator_get_value_int(iter);
    int next_number = game_number->last_round - game_number->previous_round;
    game_state_insert(state, next_number);

    free(iter);

    return state->last_number;
}    


int play_game_part1(const char* file_name, int num_rounds)
{
    GameState* state = read_input(file_name);
    num_rounds = num_rounds - state->round_number;

    for (int i = 0; i < num_rounds; ++i)
    {
        play_game_round(state);
    }

    int result = state->last_number;
    game_state_free(state);

    return result;
}


void test_read_input()
{
    GameState* state = read_input("day15_test1.txt");
    TEST_ASSERT_EQUAL(3, state->round_number);
    TEST_ASSERT_EQUAL(6, state->last_number);
    game_state_free(state);
}


void test_play_game_part1()
{
    char* file_name = "day15_test1.txt";
    TEST_ASSERT_EQUAL(0, play_game_part1(file_name, 4));
    TEST_ASSERT_EQUAL(3, play_game_part1(file_name, 5));
    TEST_ASSERT_EQUAL(3, play_game_part1(file_name, 6));
    TEST_ASSERT_EQUAL(1, play_game_part1(file_name, 7));
    TEST_ASSERT_EQUAL(0, play_game_part1(file_name, 8));
    TEST_ASSERT_EQUAL(4, play_game_part1(file_name, 9));
    TEST_ASSERT_EQUAL(0, play_game_part1(file_name, 10));
    TEST_ASSERT_EQUAL(436, play_game_part1(file_name, 2020));
}



int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_read_input);
    RUN_TEST(test_play_game_part1);
    UNITY_END();

    int result_part_one = play_game_part1("day15.txt", 2020);
    printf("Result Part 1: %d\n", result_part_one);

    int result_part_two = play_game_part1("day15.txt", 30000000);
    printf("Result Part 2: %d\n", result_part_two);

    return 0;
}

