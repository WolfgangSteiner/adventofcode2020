#include <stdio.h>
#include <stdbool.h>
#include <string.h>

bool check_password_part1(int minValue, int maxValue, char character, char* password)
{
    int numOccurences = 0;
    
    for (int i = 0; i < strlen(password); ++i)
    {
        numOccurences += (password[i] == character);
    }

    return numOccurences >= minValue && numOccurences <= maxValue;
}

bool check_password_part2(int minValue, int maxValue, char character, char* password)
{
    return (password[minValue - 1] == character) != (password[maxValue - 1] == character);
}


int main(int argc, char** argv)
{
    FILE* fp = fopen("day2.txt", "r");

    int correctCountPart1 = 0;
    int correctCountPart2 = 0;
    int minValue;
    int maxValue;
    char character;
    char password[512];

    while (fscanf(fp, "%d-%d %c: %s", &minValue, &maxValue, &character, &password) != EOF)
    {
        correctCountPart1 += check_password_part1(minValue, maxValue, character, password);
        correctCountPart2 += check_password_part2(minValue, maxValue, character, password);
    }

    printf("Part1: %d\n", correctCountPart1);
    printf("Part2: %d\n", correctCountPart2);
}
