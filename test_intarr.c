#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "intarr.h"


void setUp()
{
}


void tearDown()
{
}


void test_intarr_to_string()
{
    intarr_t* arr = intarr_init(10);
    intarr_push(arr, 3);
    intarr_push(arr, 1);
    intarr_push(arr, 4);
    intarr_push(arr, 1);
    intarr_push(arr, 5);
    char* result = intarr_to_string(arr);
    TEST_ASSERT_EQUAL_STRING("3 1 4 1 5", result);
    free(result);
    intarr_free(arr);
}


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_intarr_to_string);
    UNITY_END();

    return 0;
}

