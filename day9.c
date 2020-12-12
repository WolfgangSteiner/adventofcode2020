#include "util.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h>


bool check_number(int* array, size_t size)
{
    const int number = array[size];

    for (int i = 0; i < size - 1; ++i)
    {
        const int a = array[i];
        
        for (int j = i + 1; j < size; ++j)
        {
            const int b = array[j]; 
            if (a != b && a + b == number)
            {
                return true;
            }
        }
    }

    return false;
}

int find_first_invalid_number(const int_array* array, size_t preambleLength)
{
    for (int i = preambleLength; i < array->size; ++i)
    {
        bool isValid = check_number(&array->values[i - preambleLength], preambleLength);

        if (!isValid)
        {
            return array->values[i];
        }
    }

    return 0;
}


void find_contiguous_range(const int_array* array, int number, int* startIndex, int* endIndex)
{
    *startIndex = 0;
    *endIndex = 0;
    int currentSum = array->values[0];

    while (true)
    {
        while (currentSum < number && *endIndex < array->size - 1)
        {
            (*endIndex)++;
            currentSum += array->values[*endIndex];
        }

        if (currentSum == number)
        {
            return;
        }

        while (currentSum > number && *startIndex < array->size - 1)
        {
            currentSum -= array->values[*startIndex];
            (*startIndex)++;
        }
    }
}

void find_extrema(const int_array* array, int startIndex, int endIndex, int* minValue, int* maxValue)
{
    *minValue = INT_MAX;
    *maxValue = INT_MIN;

    for (int i = startIndex; i <= endIndex; ++i)
    {
        const int c = array->values[i];
        *minValue = MIN(*minValue, c);
        *maxValue = MAX(*maxValue, c);
    }
}


int main(int argc, char** argv)
{
    assert(argc == 2);
    int_array* array = int_array_read(argv[1]);
    const int firstInvalidNumber = find_first_invalid_number(array, 25);
    printf("First invalid number: %d\n", firstInvalidNumber);

    int startIndex = 0;
    int endIndex = 0;
    find_contiguous_range(array, firstInvalidNumber, &startIndex, &endIndex); 

    printf("StartIndex: %d, EndIndex %d\n", startIndex, endIndex); 

    int smallestNumber;
    int largestNumber;
    find_extrema(array, startIndex, endIndex, &smallestNumber, &largestNumber);
    printf("Result: %d\n", smallestNumber + largestNumber);

    return 0;
}
