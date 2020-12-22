#include <stdio.h>
#include "unity.h"
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include "strarr.h"

void setUp()
{
}


void tearDown()
{
}


void test_strarr_init_free()
{
    for (size_t i = 0; i < 10000; i++)
    {
        strarr_t* arr = strarr_init();
        strarr_free(arr);
    }
}

void test_strarr_sort()
{
    strarr_t* arr = strarr_init();
    strarr_push(arr, "ccc");
    strarr_push(arr, "zzz");
    strarr_push(arr, "bbb");
    strarr_push(arr, "aaa");
    strarr_sort(arr);

    TEST_ASSERT_EQUAL(4, arr->size);
    TEST_ASSERT_EQUAL_STRING("aaa", strarr_at(arr, 0));
    TEST_ASSERT_EQUAL_STRING("bbb", strarr_at(arr, 1));
    TEST_ASSERT_EQUAL_STRING("ccc", strarr_at(arr, 2));
    TEST_ASSERT_EQUAL_STRING("zzz", strarr_at(arr, 3));

    strarr_free(arr);
}

void test_strarr_join()
{
    strarr_t* arr = strarr_init();
    strarr_push(arr, "ccc");
    strarr_push(arr, "zzz");
    strarr_push(arr, "bbb");
    strarr_push(arr, "aaa");
    char* join = strarr_join(arr, ",");

    TEST_ASSERT_EQUAL_STRING("ccc,zzz,bbb,aaa", join);

    strarr_free(arr);
    free(join);
}

void test_strarr_remove_any()
{
    strarr_t* a = strarr_init();
    strarr_push(a, "ccc");
    strarr_push(a, "zzz");
    strarr_push(a, "bbb");
    strarr_push(a, "aaa");

    strarr_t* b = strarr_init();
    strarr_push(b, "ddd");
    strarr_push(b, "zzz");
    strarr_push(b, "eee");
    strarr_push(b, "aaa");

    strarr_remove_any(a, b);
    TEST_ASSERT_EQUAL(2, a->size);
    TEST_ASSERT_EQUAL_STRING("ccc", strarr_at(a, 0));
    TEST_ASSERT_EQUAL_STRING("bbb", strarr_at(a, 1));

    strarr_free(a);
    strarr_free(b);
}


int main()
{
    UNITY_BEGIN();
    RUN_TEST(test_strarr_init_free);
    RUN_TEST(test_strarr_sort);
    RUN_TEST(test_strarr_join);
    RUN_TEST(test_strarr_remove_any);
    UNITY_END();

    return 0;
}

