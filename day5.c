#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "util.h"

typedef struct { int start; int end; } Range;


Range binary_division(Range range, int division)
{
    const int size = range.end - range.start + 1; 
    
    if (division == 0)
    {
        return (Range) { .start=range.start, .end=range.start + size / 2 - 1 };
    }
    else
    {
        return (Range) { range.start + size / 2, range.end };
    }
}


int compute_row(const char* code)
{
    Range range = { 0, 127 };
    for (int i = 0; i < strlen(code); ++i)
    {
        int division = code[i] == 'F' ? 0 : 1;
        range = binary_division(range, division);
    }

    assert(range.start == range.end);

    return range.start;
}


int compute_column(const char* code)
{
    Range range = { 0, 7 };
    const char* column_code = code + 7;
    for (int i = 0; i < strlen(column_code); ++i)
    {
        int division = column_code[i] == 'L' ? 0 : 1;
        range = binary_division(range, division);
    }

    assert(range.start == range.end);
    return range.start;
}


int compute_id(const char* code)
{
    const int row = compute_row(code);
    const int column = compute_column(code);
    return row * 8 + column;
}


bool range_equal(Range a, Range b)
{
    return a.start == b.start && a.end && b.end;
}


void test_binary_division()
{
    assert(range_equal(binary_division((Range){0,3}, 0), (Range){0,1}));
    assert(range_equal(binary_division((Range){0,127}, 0), (Range){0,63}));
    assert(range_equal(binary_division((Range){0,127}, 1), (Range){64,127}));
    assert(range_equal(binary_division((Range){32,63}, 0), (Range){32,47}));
    assert(range_equal(binary_division((Range){32,47}, 1), (Range){40,47}));
    assert(range_equal(binary_division((Range){40,47}, 1), (Range){44,47}));
    assert(range_equal(binary_division((Range){44,47}, 0), (Range){44,45}));
    assert(range_equal(binary_division((Range){44,45}, 0), (Range){44,44}));
    assert(range_equal(binary_division((Range){0, 7}, 1), (Range){4,7}));
    assert(range_equal(binary_division((Range){4,7}, 0), (Range){4,5}));
    assert(range_equal(binary_division((Range){4,5}, 1), (Range){5,5}));
    printf("test_binary_division OK\n");
}


void test_compute_row()
{
    assert(compute_row("FBFBBFFRLR") == 44);
    assert(compute_row("BFFFBBFRRR") == 70);
    assert(compute_row("FFFBBBFRRR") == 14); 
    assert(compute_row("BBFFBBFRLL") == 102);
    printf("test_compute_row OK\n");
}

void test_compute_column()
{
    assert(compute_column("FBFBBFFRLR") == 5);
    assert(compute_column("BFFFBBFRRR") == 7);
    assert(compute_column("FFFBBBFRRR") == 7); 
    assert(compute_column("BBFFBBFRLL") == 4);
    printf("test_compute_row OK\n");
}

void test_compute_id()
{
    assert(compute_id("FBFBBFFRLR") == 357);
    assert(compute_id("BFFFBBFRRR") == 567);
    assert(compute_id("FFFBBBFRRR") == 119); 
    assert(compute_id("BBFFBBFRLL") == 820);
    printf("test_compute_row OK\n");
}


int compare_ids(const void* pa, const void* pb)
{
    const int a = *(int*)pa;
    const int b = *(int*)pb;
    return a < b ? -1 : a == b ? 0 : 1;
}


int find_free_id(int_array* array)
{
    qsort(array->values, array->size, sizeof(int), compare_ids);
    int_array_print(array);
    int last_id = 0;

    for (int i = 0; i < array->size; ++i)
    {
        const int current_id = array->values[i];
        if (last_id == 0)
        {
           last_id = current_id;
        }
        else if (current_id - last_id > 1)
        {
            assert(current_id -1 == last_id + 1);
            return current_id - 1;
        }
        else
        {
            last_id = current_id;
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    test_binary_division();
    test_compute_row();
    test_compute_column();
    test_compute_id();

    FILE* fp = fopen("day5.txt", "r");
    
    char buffer[512];
    int max_id = 0;
    int_array* ids = int_array_new();

    while (fgets(buffer, 512, fp))
    {
        buffer[strlen(buffer) - 1] = '\0';
        const int id = compute_id(buffer);
        int_array_push_back(ids, id);

        printf("%s -> %d\n", buffer, id);
        if (id > max_id) max_id = id;
    }

    printf("Highest ID: %d\n", max_id);

    int my_id = find_free_id(ids);
    printf("My seat: %d\n", my_id);

    return 0;
}
