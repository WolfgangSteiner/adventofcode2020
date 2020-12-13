#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"


int_array* parse_input(char* filename)
{
    int_array* result = int_array_new();
    FILE* fp = fopen(filename, "r");
    int line_length = 2048;
    char* line = malloc(line_length);
    fgets(line, line_length, fp);
    int_array_push_back(result, atoi(line));    
    char* pos = line;
    char* endptr = 0;
    fgets(line, line_length, fp);
    chomp(line); 
    while (*pos != '\0')
    {
        if (*pos == 'x')
        {
            int_array_push_back(result, 0);
            pos++;
            if (*pos == ',') pos++;
            continue;
        }

        long int number = strtol(pos, &endptr, 10);
        assert(endptr != pos);
        int_array_push_back(result, (int)number);
        pos = endptr;
        if (*pos == ',') pos++;
    }

    free(line);

    return result;
}


int evaluate_input(const int_array* input)
{
    const int timestamp = int_array_at(input, 0);
    int min_timestamp = INT_MAX;
    int min_bus_id = INT_MAX;

    for (int i = 1; i < input->size; ++i)
    {
        const int bus_id = int_array_at(input, i);
        if (bus_id == 0) continue;
        int t = (int)(ceil((float)timestamp / bus_id) * bus_id); 

        if (t < min_timestamp)
        {
            min_timestamp = t;
            min_bus_id = bus_id;
        }
    }

    return (min_timestamp - timestamp) * min_bus_id; 
}


int count_bus_ids(const int_array* input)
{
    int result = 0;

    for (int i = 1; i < input->size; ++i)
    {
        if (int_array_at(input, i) > 0) result++;
    }

    return result;
}


typedef struct 
{
    int bus_id;
    int offset;
} Entry;


typedef struct
{
    int size;
    Entry* entries;
} EntryArray;


EntryArray* transform_input(const int_array* input)
{
    EntryArray* r = calloc(1, sizeof(EntryArray));
    int num_entries = count_bus_ids(input);
    r->size = num_entries;
    r->entries = calloc(num_entries, sizeof(Entry));
    int bus_id_count = 0;

    for (int i = 1; i < input->size; ++i)
    {
        const int bus_id = int_array_at(input, i);
        if (bus_id > 0)
        {
            Entry* e = &r->entries[bus_id_count];
            e->bus_id = bus_id;
            e->offset = i - 1;
            bus_id_count++;
        }
    }

    return r;
}



bool entries_equal(const Entry a, int bus_id, int offset)
{
    return a.offset == offset && a.bus_id == bus_id;
}


int compare_entries(const void* pa, const void* pb)
{
    const int a = ((Entry*)pa)->bus_id;
    const int b = ((Entry*)pb)->bus_id;
    return a < b ? -1 : a == b ? 0 : 1;
}


void sort_entries(EntryArray* entries)
{
    qsort(entries->entries, entries->size, sizeof(Entry), compare_entries);
}


long long evaluate_part2(const int_array* input)
{
    EntryArray* entries = transform_input(input);
    sort_entries(entries);

    long long first_factor = entries->entries[0].bus_id;
    long long factor = first_factor;
    long long result = 0;

    for (int i = 1; i < entries->size; ++i)
    {
        const Entry* e = &entries->entries[i];
        int next_factor = e->bus_id;
        int offset = e->offset;

        while ((result + offset) % next_factor)
        {
            result += factor;
        }
        factor *= next_factor;
    }

    return result;    
}

int_array* test_input;

void setUp()
{
    test_input = parse_input("day13_test.txt");
}


void tearDown()
{
    int_array_free(test_input);
}


void test_parse_input()
{
   TEST_ASSERT_EQUAL(939, int_array_at(test_input, 0)); 
   TEST_ASSERT_EQUAL(7,   int_array_at(test_input, 1)); 
   TEST_ASSERT_EQUAL(13,  int_array_at(test_input, 2)); 
   TEST_ASSERT_EQUAL(0,   int_array_at(test_input, 3)); 
   TEST_ASSERT_EQUAL(0,   int_array_at(test_input, 4)); 
   TEST_ASSERT_EQUAL(59,  int_array_at(test_input, 5)); 
   TEST_ASSERT_EQUAL(0,   int_array_at(test_input, 6)); 
   TEST_ASSERT_EQUAL(31,  int_array_at(test_input, 7)); 
   TEST_ASSERT_EQUAL(19,  int_array_at(test_input, 8)); 
}


void test_result()
{
    TEST_ASSERT_EQUAL(295, evaluate_input(test_input));
}


void test_transform_input()
{
    EntryArray* e = transform_input(test_input);
    TEST_ASSERT_EQUAL(5, e->size);
    TEST_ASSERT(entries_equal(e->entries[0], 7, 0));
    TEST_ASSERT(entries_equal(e->entries[1], 13, 1));
    TEST_ASSERT(entries_equal(e->entries[2], 59, 4));
    TEST_ASSERT(entries_equal(e->entries[3], 31, 6));
    TEST_ASSERT(entries_equal(e->entries[4], 19, 7));
}

void test_sort_entries()
{
    EntryArray* e = transform_input(test_input);
    sort_entries(e);
    TEST_ASSERT_EQUAL(5, e->size);
    TEST_ASSERT(entries_equal(e->entries[0], 7, 0));
    TEST_ASSERT(entries_equal(e->entries[1], 13, 1));
    TEST_ASSERT(entries_equal(e->entries[2], 19, 7));
    TEST_ASSERT(entries_equal(e->entries[3], 31, 6));
    TEST_ASSERT(entries_equal(e->entries[4], 59, 4));
}

void test_evaluate_part2()
{
    long long result = evaluate_part2(test_input);
    TEST_ASSERT_EQUAL(1068781, result);
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_parse_input);
    RUN_TEST(test_result);
    RUN_TEST(test_transform_input);
    RUN_TEST(test_sort_entries);
    RUN_TEST(test_evaluate_part2);
    UNITY_END();

    int_array* input = parse_input("day13.txt");
    int result = evaluate_input(input);
    printf("Result: %d\n", result);

    long long part2 = evaluate_part2(input);
    printf("Result part 2: %lld\n", part2);

    return 0;
}

