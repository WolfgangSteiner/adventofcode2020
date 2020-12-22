#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "hash_map.h"
#include "strarr.h"


void setUp()
{
}


void tearDown()
{
}

typedef struct
{
    HashMap* ingredient_map;
    HashMap* allergen_map;
} problem_t;

problem_t* problem_init()
{
    problem_t* p = calloc(1, sizeof(problem_t));
    p->ingredient_map = hash_map_init(4096);
    p->allergen_map = hash_map_init(128);
    return p;
}


void parse_line(char* line, strarr_t* ingredients, strarr_t* allergens)
{
    strarr_clear(ingredients);
    strarr_clear(allergens);
    char* buffer = malloc(strlen(line) + 1);
    strcpy(buffer, line);
    char* word = strtok(buffer, " ");
    strarr_push(ingredients, word); 
    bool parse_ingredients = true;
    char allergen[128];

    while ((word = strtok(NULL, " ")))
    {
        if (word[0] == '(')
        {
            parse_ingredients = false;
            continue;
        }
        else if (parse_ingredients)
        {
            strarr_push(ingredients, word);
        }
        else
        {
            strcpy(allergen, word);
            size_t allergen_length = strlen(allergen);
            char c = allergen[allergen_length - 1];
            MUnused(c);
            assert(c == ',' || c == ')');
            allergen[allergen_length - 1] = '\0';                
            strarr_push(allergens, allergen);
        }
    }

    free(buffer);
}


void update_ingredient(HashMap* ingredient_map, char* ingredient)
{
    HashMapIterator* iter = hash_map_find(ingredient_map, ingredient);
    if (hash_map_iterator_is_end(iter))
    {
        hash_map_insert(ingredient_map, ingredient, (void*)1u);
    }
    else
    {
        iter->bucket->value++;
    }

    free(iter);
}


void update_allergen(HashMap* allergen_map, char* allergen, strarr_t* ingredients)
{
    HashMapIterator* iter = hash_map_find(allergen_map, allergen);
    if (hash_map_iterator_is_end(iter))
    {
        hash_map_insert(allergen_map, allergen, strarr_copy(ingredients));
    }  
    else
    {
        strarr_t* possible_ingredients = hash_map_iterator_get_value(iter);
        strarr_t* intersection = strarr_intersect(possible_ingredients, ingredients);
        strarr_free(possible_ingredients);
        iter->bucket->value = intersection;
    }
    free(iter);
}

void process_input(problem_t* problem, strarr_t* ingredients, strarr_t* allergens)
{
    for (size_t i = 0; i < ingredients->size; ++i)
    {
        char* ingredient = strarr_at(ingredients, i);
        update_ingredient(problem->ingredient_map, ingredient); 
    }

    for (size_t i = 0; i < allergens->size; ++i)
    {
        char* allergen = strarr_at(allergens, i);
        update_allergen(problem->allergen_map, allergen, ingredients);
    }
}




problem_t* parse_input(char* file_name)
{
    FILE* fp = fopen(file_name, "r");
    problem_t* problem = problem_init();
    strarr_t* ingredients = strarr_init();
    strarr_t* allergens = strarr_init();
    char line[4096];

    while (!feof(fp))
    {
        if (fgets(line, 4096, fp))
        {
            chomp(line);
            parse_line(line, ingredients, allergens);
            process_input(problem, ingredients, allergens);
        }
    }

    return problem;
}


bool contains_allergen(problem_t* problem, char* ingredient)
{
    bool result = false;

    HashMapIterator* iter = hash_map_iterator_begin(problem->allergen_map);
    while (!hash_map_iterator_is_end(iter))
    {
        strarr_t* possible_ingredients = hash_map_iterator_get_value(iter);
        if (strarr_contains(possible_ingredients, ingredient))
        {
            result = true;
            break;
        }
        hash_map_iterator_next(iter);
    }

    free(iter);
    return result;
}


size_t evaluate_part_one(char* file_name)
{
    problem_t* problem = parse_input(file_name);
    size_t result = 0; 
    HashMapIterator* iter = hash_map_iterator_begin(problem->ingredient_map);
    while(!hash_map_iterator_is_end(iter))
    {
        char* ingredient = hash_map_iterator_get_key(iter);
        if (!contains_allergen(problem, ingredient))
        {
            size_t count = (size_t)hash_map_iterator_get_value(iter);
            result += count;
        }
        hash_map_iterator_next(iter);
    }

    free(iter);

    return result;

}


bool has_unsolved_ingredients(problem_t* problem)
{
    bool result = false;
    HashMapIterator* iter = hash_map_iterator_begin(problem->allergen_map);
    while (!hash_map_iterator_is_end(iter))
    {
        strarr_t* ingredients = hash_map_iterator_get_value(iter);
        if (ingredients->size > 1)
        {
            result = true;
            break;
        }
        hash_map_iterator_next(iter);
    }

    free(iter);
    return result;
}


void collect_solved_ingredients(problem_t* problem, strarr_t* solved_ingredients)
{
    HashMapIterator* iter = hash_map_iterator_begin(problem->allergen_map);
    while (!hash_map_iterator_is_end(iter))
    {
        strarr_t* ingredients = hash_map_iterator_get_value(iter);
        if (ingredients->size == 1)
        {
            char* ingredient = strarr_at(ingredients, 0); 

            if (!strarr_contains(solved_ingredients, ingredient))
            {
                strarr_push(solved_ingredients, ingredient);
            }
        }
        hash_map_iterator_next(iter);
    }

    free(iter);
}

void solve_ingredients(problem_t* problem, strarr_t* solved_ingredients)
{
    HashMapIterator* iter = hash_map_iterator_begin(problem->allergen_map);
    while (!hash_map_iterator_is_end(iter))
    {
        strarr_t* ingredients = hash_map_iterator_get_value(iter);
        if (ingredients->size > 1)
        {
            strarr_remove_any(ingredients, solved_ingredients);
        }
        hash_map_iterator_next(iter);
    }

    free(iter);
}


void solve_problem(problem_t* problem)
{
    strarr_t* solved_ingredients = strarr_init();
    
    while (has_unsolved_ingredients(problem))
    {
        collect_solved_ingredients(problem, solved_ingredients);
        solve_ingredients(problem, solved_ingredients);
    }
}


char* evaluate_part_two(char* file_name)
{
    problem_t* problem = parse_input(file_name);
    solve_problem(problem);
    strarr_t* allergens = hash_map_get_keys(problem->allergen_map);
    strarr_sort(allergens);
    strarr_t* result_array = strarr_init();

    for (size_t i = 0; i < allergens->size; i++)
    {
        char* allergen = strarr_at(allergens, i);
        strarr_t* ingredients = hash_map_get_value(problem->allergen_map, allergen);
        assert(ingredients);
        char* ingredients_str = strarr_join(ingredients, ", ");
        printf("Ingredients for allergen %s: %s\n", allergen, ingredients_str);
        free(ingredients_str);
        assert(ingredients->size == 1);
        strarr_push(result_array, strarr_at(ingredients, 0));
    }

    char* result = strarr_join(result_array, ",");

    strarr_free(allergens);
    strarr_free(result_array);

    return result; 
}


void test_parse_line()
{
    strarr_t* ingredients = strarr_init();
    strarr_t* allergens = strarr_init();
    char* line = "mxmxvkd kfcds sqjhc nhms (contains dairy, fish)";
    parse_line(line, ingredients, allergens);
    TEST_ASSERT_EQUAL(4, ingredients->size);
    TEST_ASSERT_EQUAL(2, allergens->size);
    TEST_ASSERT_EQUAL_STRING("mxmxvkd", strarr_at(ingredients, 0));
    TEST_ASSERT_EQUAL_STRING("kfcds", strarr_at(ingredients, 1));
    TEST_ASSERT_EQUAL_STRING("sqjhc", strarr_at(ingredients, 2));
    TEST_ASSERT_EQUAL_STRING("nhms", strarr_at(ingredients, 3));
    TEST_ASSERT_EQUAL_STRING("dairy", strarr_at(allergens, 0));
    TEST_ASSERT_EQUAL_STRING("fish", strarr_at(allergens, 1));
    strarr_free(ingredients);
    strarr_free(allergens);
}


void test_evaluate_part_one()
{
    size_t result = evaluate_part_one("day21_test.txt");
    TEST_ASSERT_EQUAL(5, result);
}


void test_evaluate_part_two()
{
    char* result = evaluate_part_two("day21_test.txt");
    TEST_ASSERT_EQUAL_STRING("mxmxvkd,sqjhc,fvjkl", result);
}

int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_line);
    RUN_TEST(test_evaluate_part_one);
    RUN_TEST(test_evaluate_part_two);
    UNITY_END();

    printf("Result part 1: %lu", evaluate_part_one("day21.txt"));

    char* result_part_two = evaluate_part_two("day21.txt");
    printf("Result part 2: %s\n", result_part_two);
    free(result_part_two);

    return 0;
}

