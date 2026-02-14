// Whitespace Interpreter
// If no arguments are passed, it should act as a shell, similar to JShell or just typing Python in the terminal
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define NOCOLOR "\x1b[0m"
#define RED     "\x1b[41m"
#define GREEN   "\x1b[42m"
#define YELLOW  "\x1b[43m"
#define BLUE    "\x1b[44m"
#define MAGENTA "\x1b[45m"
#define CYAN    "\x1b[46m"

int main(int argc, char* argv[])
{
    FILE *code_file;

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
    else if (strcmp(argv[1], "cws") == 0)
    {
        // printf("Convert to character notation [s - space, t - tab, l - line feed]\n");
        code_file = fopen(argv[2]);
    }

    else
    {
        printf("Not a valid option\n");
    }

    return 0;
}