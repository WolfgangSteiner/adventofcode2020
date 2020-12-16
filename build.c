#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

typedef struct
{
    bool debug;
    char* file_name;
} Arguments;


Arguments* parse_command_line(int argc, char** argv)
{
    Arguments* args = calloc(1, sizeof(Arguments));

    for (int i = 1; i < argc; ++i)
    {
        char* argument = argv[i];

        if (!strcmp(argument, "--debug"))
        {
            args->debug = true;
        }
        else
        {
            args->file_name = argument;
        }
    }

    if (args->file_name == NULL)
    {
        printf("Please provide program name.");
        exit(1);
    }

    return args;
}


int main(int argc, char** argv)
{
    Arguments* args = parse_command_line(argc, argv);

    char cmd[1024];
    system("mkdir -p bin");

    const char* src_files = "Unity/src/unity.c util.c hash_map.c hash_map_int.c";
    const char* build_options = "-g -O2";
    const char* warnings = "-Wall -Werror";
    const char* include_dirs = "-IUnity/src";
    int result = 1;
    snprintf(cmd, 1024, "gcc %s %s %s -o bin/%s %s.c %s",
            build_options, warnings, include_dirs, args->file_name, args->file_name, src_files);

    while (result)
    {
        system("clear");
        result = system(cmd); 
        sleep(1);
    }

    if (result == 0)
    {
        if (args->debug)
        {
            snprintf(cmd, 1024, "lldb bin/%s", args->file_name);
        }
        else
        {
            snprintf(cmd, 1024, "time bin/%s", args->file_name);
        }

        system(cmd);
    }

}
