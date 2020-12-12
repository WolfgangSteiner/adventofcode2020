#include <stdio.h>
#include <stdbool.h>
#include <string.h>


int countTrees(int incX, int incY)
{
    FILE* fp = fopen("day3.txt", "r");

    char map[512];
    int numTrees = 0;
    int px = 0;
    int py = 0;

    while (fscanf(fp, "%s", &map) != EOF)
    {
        if (py % incY == 0)
        { 
            int size = strlen(map);
            numTrees += map[px] == '#';
            px = (px + incX) % size;
        }
        py ++;
    }

    fclose(fp);

    return numTrees;
}



int main(int argc, char** argv)
{
    typedef struct { int xi; int yi; } Slope;
    Slope slopes[] = {{1,1}, {3,1}, {5,1}, {7,1}, {1,2}};

    long long int result = 1;

    for (int i = 0; i < 5; ++i)
    {
        Slope slope = slopes[i];
        int count = countTrees(slope.xi, slope.yi);
        result *= count;
        printf("Slope %d/%d: %d\n", slope.xi, slope.yi, count); 
    }

    printf("Result: %lld\n", result);

    return 0;
}
