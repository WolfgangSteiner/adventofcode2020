#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>


int count_unique(const char* record)
{
    int answers[26];
    memset(answers, 0, 26 * sizeof(int));

    for (int i = 0; i < strlen(record); ++i)
    {
        const char c = record[i];
        
        if (c >= 'a' && c <= 'z')
        {
            const int bin = c - 'a';
            answers[bin] = 1;
        }
    }

    int result = 0;

    for (int i = 0; i < 26; ++i)
    {
        result += answers[i];
    }

    return result;
}
       
int count_common(const char* record)
{
    int answers[26];
    int numPersons = 0;
    memset(answers, 0, 26 * sizeof(int));


    for (int i = 0; i < strlen(record); ++i)
    {
        const char c = record[i];
        
        if (c >= 'a' && c <= 'z')
        {
            const int bin = c - 'a';
            answers[bin] += 1;
        }
        else if (c == '\n')
        {
            numPersons += 1;
        }
    }

    int result = 0;

    for (int i = 0; i < 26; ++i)
    {
        result += answers[i] == numPersons;
    }

    return result;
}

int main(int argc, char** argv)
{
    FILE* fp = fopen("day6.txt", "r");
    char line[512];
    char record[2048];
    record[0] = '\0';            

    int totalUniqueCount = 0;
    int totalCommonCount = 0;

    while (true)
    {
        fgets(line, 512, fp);

        if (feof(fp) || !strcmp(line, "\n"))
        {
            totalUniqueCount += count_unique(record);
            totalCommonCount += count_common(record);
            record[0] = '\0';            
        }
        else
        {
            strcat(record, line);
        }

        if (feof(fp)) break;
    }

    printf("Total count: %d\n", totalUniqueCount);
    printf("Common count: %d\n", totalCommonCount);
}
