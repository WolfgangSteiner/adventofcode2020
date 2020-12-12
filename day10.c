#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "unity.h"

void count_differences(const int_array* jolts, int* differences)
{
    memset(differences, 0, sizeof(int) * 3);

    for (int i = 1; i < jolts->size; ++i)
    {
        const int jolt = jolts->values[i];
        const int previousJolt = jolts->values[i-1];
        const int diff = jolt - previousJolt;
        differences[diff - 1]++;
    }
}


int binomial_coefficient(int n, int k)
{
    double result = 1.0;

    for (int i = 1; i <= k; ++i)
    {
        result *= (n + 1 - i) / (double)i;
    }

    return (int) result + 0.01;
}


int compute_difference(const int* element)
{
    return *(element + 1) - *element;
}


bool get_next_sub_chain(const int_array* jolts, int* p_startIndex, int* p_endIndex)
{
    int startIndex = *p_startIndex;
    int endIndex = *p_endIndex;

    if (startIndex >= jolts->size - 1) return false;

    const int* arrayStart = jolts->values;
    const int* arrayEnd = jolts->values + jolts->size;
    const int* startPtr = &jolts->values[*p_startIndex];

    while (compute_difference(startPtr) > 1)
    {
        startPtr++;
        if (startPtr >= arrayEnd) return false;
    }

    const int* endPtr = startPtr;

    while (compute_difference(endPtr) == 1)
    {
        endPtr++;
    }

    *p_startIndex = startPtr - arrayStart;
    *p_endIndex = endPtr - arrayStart;
    return true;
}

int compute_permutations_for_sub_chain(int length)
{
    if (length <= 2) return 1;

    int n = length - 2;
    int res = 0;

    for (int i = 0; i <= n; ++i)
    {
        res += binomial_coefficient(n, i);
        if (i > 2)
        {
            res -= (n - i + 1);
        }
    }

    return res;
}


long long count_permutations(const int_array* jolts)
{
    int startIndex = 0;
    int endIndex = 0;
    
    long long result = 1;

    while (get_next_sub_chain(jolts, &startIndex, &endIndex))
    {
        printf("SubChain: ");
        for (int i = startIndex; i <= endIndex; ++i)
        {
            printf(" %d ", jolts->values[i]);
        }

        const int chainLength = endIndex - startIndex + 1;
        const int permutations = compute_permutations_for_sub_chain(chainLength);
        result *= permutations;
        printf("-> length: %d, permutations: %d\n", chainLength, permutations);

        startIndex = endIndex;
    }

    return result;
}
    

void setUp() {}
void tearDown() {}


void test_binomial_coefficient()
{
    TEST_ASSERT_EQUAL(1, binomial_coefficient(2, 0));
    TEST_ASSERT_EQUAL(2, binomial_coefficient(2, 1));
    TEST_ASSERT_EQUAL(1, binomial_coefficient(2, 2));
}


int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_binomial_coefficient);
    UNITY_END();


    int_array* jolts = int_array_read(argv[1]);
    int_array_sort(jolts);

    const int outputJolt = int_array_back(jolts) + 3;
    int_array_push_front(jolts, 0);
    int_array_push_back(jolts, outputJolt);
    int_array_print(jolts);
    int differences[3];
    count_differences(jolts, differences);

    for (int i = 0; i < 3; ++i)
    {
        printf("%d: %d\n", i + 1, differences[i]);
    }

    printf("Result: %d\n", differences[0] * differences[2]);

    long long result = count_permutations(jolts);
    printf("Permutations: %lld\n", result);

    return 0;
}
