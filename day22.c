#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "intarr.h"
#include "ctype.h"
#include "string.h"
#include "strarr.h"


void setUp()
{
}


void tearDown()
{
}

typedef struct 
{
    intarr_t* player_a;
    intarr_t* player_b;
} game_t;

static int s_game_number = 0;

game_t* game_init()
{
    return calloc(1, sizeof(game_t));
}

void game_free(game_t* game)
{
    intarr_free(game->player_a);
    intarr_free(game->player_b);
    free(game);
}


intarr_t* parse_player_input(FILE* fp)
{
    intarr_t* cards = intarr_init(64);
    char line[512];

    while(!feof(fp))
    {
        if (fgets(line, 512, fp))
        {
            chomp(line);
            if (isdigit(line[0]))
            {
                intarr_push(cards, atoi(line));
            }
            else
            {
                break;
            }
        }
    } 

    return cards;
}


bool play_round(game_t* game)
{
    assert(game->player_a->size && game->player_b->size);
    int card_a = intarr_pop(game->player_a);
    int card_b = intarr_pop(game->player_b);

    if (card_a >= card_b)
    {
        intarr_push(game->player_a, card_a);
        intarr_push(game->player_a, card_b);
    }
    else
    {
        intarr_push(game->player_b, card_b);
        intarr_push(game->player_b, card_a);
    }

    return (game->player_a->size && game->player_b->size);
}



game_t* parse_input(char* file_name)
{
    FILE* fp = fopen(file_name, "r");
    char line[512];
    game_t* game = game_init(); 

    fgets(line, 512, fp);
    assert(strstr(line, "Player 1"));
    game->player_a = parse_player_input(fp);
    fgets(line, 512, fp);
    assert(strstr(line, "Player 2"));
    game->player_b = parse_player_input(fp);

    fclose(fp);
    return game;
}


size_t calculate_score_for_stack(intarr_t* stack)
{
    size_t result = 0;

    for (size_t i = 0; i < stack->size; ++i)
    {
        size_t factor = stack->size - i;
        result += factor * stack->data[i];
    }

    return result;
}


size_t calculate_score(game_t* game)
{
    return MAX(calculate_score_for_stack(game->player_a), calculate_score_for_stack(game->player_b));
}


size_t play_game(char* file_name)
{
    game_t* game = parse_input(file_name);
    while (play_round(game));

    size_t result = calculate_score(game);
    game_free(game);
    return result;
}


int play_recursive_game(intarr_t* stack_a, intarr_t* stack_b, size_t num_cards_a, size_t num_cards_b);



int play_recursive_game_round(
    intarr_t* stack_a, intarr_t* stack_b,
    strarr_t* previous_stacks_a, strarr_t* previous_stacks_b,
    int round_number,
    int game_number)
{
    printf("Player 1's deck: "); intarr_print(stack_a); 
    printf("Player 2's deck: "); intarr_print(stack_b);

    char* current_deck_str_a = intarr_to_string(stack_a);
    MUnused(previous_stacks_b);

    if (strarr_contains(previous_stacks_a, current_deck_str_a))
    {
        printf("Previous deck detected: %s\n", current_deck_str_a);
        free(current_deck_str_a);
        return 1;
    }
    else
    {
        strarr_push(previous_stacks_a, current_deck_str_a);
        free(current_deck_str_a);
    }
    

    int card_a = intarr_pop(stack_a);
    int card_b = intarr_pop(stack_b);
    printf("Player 1 plays %d\n", card_a);
    printf("Player 2 plays %d\n", card_b);


    int result = 0;
    
    if (card_a <= (int)stack_a->size && card_b <= (int)stack_b->size)
    {
        printf("Playing a sub-game to determine the winner...\n");
        result = play_recursive_game(stack_a, stack_b, card_a, card_b);
        printf("... back to round %d of game %d\n", round_number, game_number);
    }  
    else
    {
        result = card_a >= card_b ? 1 : -1;
    }

    if (result == 1)
    {
        printf("Player 1 wins round %d of game %d!\n\n", round_number, game_number);
        intarr_push(stack_a, card_a);
        intarr_push(stack_a, card_b);
    }
    else if (result == -1)
    {
        printf("Player 2 wins round %d of game %d!\n\n", round_number, game_number);
        intarr_push(stack_b, card_b);
        intarr_push(stack_b, card_a);
    }
    else
    {
        MInvalid();
    }

    return stack_a->size == 0 ? -1 : stack_b->size == 0 ? 1 : 0;
}


intarr_t* copy_stack(intarr_t* stack, size_t num_cards)
{
    intarr_t* copy = intarr_init(num_cards);
    memcpy(copy->data, stack->data, sizeof(int) * num_cards);
    copy->size = num_cards; 
    return copy;
}


int play_recursive_game(intarr_t* stack_a, intarr_t* stack_b, size_t num_cards_a, size_t num_cards_b)
{
    printf("====== Game %d =======\n", s_game_number++);
    int game_number = s_game_number;
    intarr_t* new_stack_a = copy_stack(stack_a, num_cards_a);
    intarr_t* new_stack_b = copy_stack(stack_b, num_cards_b);
    strarr_t* previous_stacks_a = strarr_init();
    strarr_t* previous_stacks_b = strarr_init();
    int result;
    int round_number = 1;
    while (true)
    {
        printf("-- Round %d (Game %d) --\n", round_number, game_number);
        result = play_recursive_game_round(
                new_stack_a, new_stack_b,
                previous_stacks_a, previous_stacks_b,
                round_number++, game_number);
        if (result != 0) break;
    }
    intarr_free(new_stack_a);
    intarr_free(new_stack_b);
    strarr_free(previous_stacks_a);
    strarr_free(previous_stacks_b);

    return result;
}


size_t play_game_part_two(char* file_name)
{
    game_t* game = parse_input(file_name);
    strarr_t* previous_stacks_a = strarr_init();
    strarr_t* previous_stacks_b = strarr_init();
    s_game_number = 1;
    int game_number = s_game_number;
    int round_number = 1;
    while (true)
    {
        printf("-- Round %d (Game %d) --\n", round_number, game_number);
        int result = play_recursive_game_round(
            game->player_a, game->player_b,
            previous_stacks_a, previous_stacks_b,
            round_number++, game_number);
        if (result != 0) break;
    }

    size_t result = calculate_score(game);
    strarr_free(previous_stacks_a);
    strarr_free(previous_stacks_b);
    game_free(game);
    return result;
}


void test_parse_input()
{
    game_t* game = parse_input("day22_test.txt");
    TEST_ASSERT_EQUAL(5, game->player_a->size);    
    TEST_ASSERT_EQUAL(5, game->player_b->size);    
    game_free(game);
}

void test_play_game()
{
    TEST_ASSERT_EQUAL(306, play_game("day22_test.txt"));
}


void test_play_game_part_two()
{
    TEST_ASSERT_EQUAL(291, play_game_part_two("day22_test.txt"));
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_input);
    RUN_TEST(test_play_game);
    RUN_TEST(test_play_game_part_two);
    UNITY_END();

    printf("Result Part 1: %lu\n", play_game("day22.txt"));
    printf("Result Part 2: %lu\n", play_game_part_two("day22.txt"));


    return 0;
}

