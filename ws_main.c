#include "whitespace.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// TODO: Configure each of the options to handle their own processes
int main(int argc, char *argv[])
{
    FILE    *source_file;

    /* Shell */
    if (argc == 1)
    {
        print_help();
    }

    /* Interpreter */
    else if (strcmp(argv[1], "run") == 0)
    {
        source_file = fopen(argv[2], "r");
        if (ws_load(source_file) == 0)
        {
            if (ws_run() == 1)
            {
                exit(1);
            }
        }
    }
    
    /* Converter */
    else if (strcmp(argv[1], "cws") == 0 && argc > 2)
    {
        ws_convert(argv[2]);
    }

    /* Echo the Whitespace program */
    else if (strcmp(argv[1], "echo") == 0 && argc > 2)
    {
        ws_echo(argv[2]);
    }

    else
    {
        printf("Not a valid option\n");
    }

    return 0;
}