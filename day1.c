#include "util.h"
#include <stdio.h>

int main(int argc, char** argv)
{
    int_array* numbers = int_array_read("day1.txt");

    for (int i = 0; i < numbers->size - 2; ++i)
    {
        const int value1 = numbers->values[i];

        for (int j = i+1; j < numbers->size - 1; ++j)
        {
            const int value2 = numbers->values[j];

            for (int k = j+1; k < numbers->size - 1; ++k)
            {
                const int value3 = numbers->values[k];

                if (value1 + value2 + value3 == 2020)
                {
                    printf("Result: %d\n", value1 * value2 * value3);
                    return 0;
                }
            }
        }
    }

    return 0;
}
