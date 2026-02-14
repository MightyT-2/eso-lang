// Whitespace Interpreter
// If no arguments are passed, it should act as a shell, similar to JShell or just typing Python in the terminal
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define NOCOLOR      "\x1b[0m"
#define RED_FORE     "\x1b[31m"
#define GREEN_FORE   "\x1b[32m"
#define YELLOW_FORE  "\x1b[33m"
#define BLUE_FORE    "\x1b[34m"
#define MAGENTA_FORE "\x1b[35m"
#define CYAN_FORE    "\x1b[36m"
#define RED_BACK     "\x1b[41m"
#define GREEN_BACK   "\x1b[42m"
#define YELLOW_BACK  "\x1b[43m"
#define BLUE_BACK    "\x1b[44m"
#define MAGENTA_BACK "\x1b[45m"
#define CYAN_BACK    "\x1b[46m"

int main(int argc, char* argv[])
{
    FILE *code_file;
    char next_char;
    int  char_counter = 0;

    /* Shell */
    if (argc == 1)
    {
        printf("A Whitespace Shell\n");

    }

    /* Interpreter */
    else if (strcmp(argv[1], "run") == 0)
    {
        printf("Run a program from a file\n");
    }
    
    /* Converter */
    else if (strcmp(argv[1], "cws") == 0 && argc > 2)
    {
        // printf("Convert to character notation [s - space, t - tab, l - line feed]\n");
        code_file = fopen(argv[2], "r");
        while ((next_char = fgetc(code_file)) != EOF)
        {
            if (next_char == (int) ' ')
            {
                printf(RED_FORE "s" NOCOLOR);
            }
            else if (next_char == (int) '\t')
            {
                printf(BLUE_FORE "t" NOCOLOR);
            }
            else if (next_char == (int) '\n')
            {
                printf(GREEN_FORE "l" NOCOLOR);
            }
        }
        printf("\n");
    }

    /* Echo the Whitespace program */
    else if (strcmp(argv[1], "echo") == 0 && argc > 2)
    {
        code_file = fopen(argv[2], "r");
        while ((next_char = fgetc(code_file)) != EOF)
        {
            if (next_char == (int) ' ')
            {
                printf(RED_BACK " " NOCOLOR);
                char_counter++;
            }
            else if (next_char == (int) '\t')
            {
                do
                {
                    printf(BLUE_BACK " " NOCOLOR);
                    char_counter++;
                } while (char_counter % 8 != 0);
            }
            else if (next_char == (int) '\n')
            {
                printf(GREEN_BACK " " NOCOLOR "\n");
                char_counter = 0;
            }
        }
    }

    else
    {
        printf("Not a valid option\n");
    }

    return 0;
}