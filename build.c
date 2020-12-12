#include <stdlib.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Please provide program name.");
        return 1;
    }

    char* file_name = argv[1];
    char cmd[1024];

    snprintf(cmd, 1024, "gcc -o %s -Wall -Werror -O2 -IUnity/src %s.c Unity/src/unity.c util.c", file_name, file_name);
    system(cmd);
}
