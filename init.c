#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "util.h"
#include <unistd.h>


int main(int argc, char** argv)
{
    if (argc < 2)
    {
        printf("Please provide a name for the new program.\n");
        return(1);
    }

    char* file_name = argv[1];

    if (strcmp(file_name + strlen(file_name) - 2, ".c"))
    {
       char* new_file_name = malloc(strlen(file_name) + 2 + 1);
       sprintf(new_file_name, "%s.c", file_name);
       file_name = new_file_name;
    }

    if(access(file_name, F_OK) == 0)
    {
        printf("File %s already exists.\n", file_name);
        return(1);
    }

    puts(file_name);

    char cmd[512];
    snprintf(cmd, 512, "cp template.c %s", file_name);
    printf("template.c -> %s\n", file_name);
    system(cmd);
    
    snprintf(cmd, 512, "git add %s; git commit -m \"Add %s\"", file_name, file_name);
    system(cmd);

    return 0;
}


