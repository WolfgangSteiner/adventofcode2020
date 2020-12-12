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
    
    system("mkdir -p bin");

    snprintf(cmd, 1024, "gcc -o bin/%s -Wall -Werror -O2 -IUnity/src %s.c Unity/src/unity.c util.c", file_name, file_name);
    int result = system(cmd);

    if (result == 0)
    {
        snprintf(cmd, 1024, "time bin/%s", file_name);
        system(cmd);
    }

}
