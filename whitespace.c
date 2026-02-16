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

typedef struct strchar
{
    char    character;
    strchar *next_character;
} strchar;

typedef struct instruction
{
    instruction *next_instruction;
    int         instruction_number,
                parameter;
} instruction;

typedef struct label
{
    instruction *label_location;
    label       *next_label,
                *previous_label;
} label;

typedef struct process
{
    instruction *first_instruction;
    label       *first_label,
                *last_label,
                *current_label;
} process;

int ws_shell()
{
    printf("A Whitespace Shell\n");
}

char find_next_token(FILE *source_file)
{
    char next_char;
    do
    {
        next_char = fgetc(source_file);
    } while(next_char != ' ' && next_char != '\t' && next_char != '\n' && next_char != EOF);
    
    return next_char;
}

int ws_interpret(char *file_location)
{
    // printf("Run a program from a file\n");
    FILE    *source_file;
    char    next_char;
    process ws_process;

    source_file = fopen(file_location, "r");
    while ((next_char = find_next_token(source_file)) != EOF)
    {


        /* Stack Manipulation */
        if (next_char == ' ')
        {
            // | Command     | Parameters | Meaning                             |
            // |-------------|------------|-------------------------------------|
            // | [Space]     | Number     | Push the number onto the stack      |
            // | [LF][Space] | -          | Duplicate the top item on the stack |
            // | [LF][Tab]   | -          | Swap the top two items on the stack |
            // | [LF][LF]    | -          | Discard the top item on the stack   |
        }
        
        /* Tab */
        else if (next_char == '\t')
        {
            
            /* Arithmetic */
            if (next_char == ' ')
            {
                // | Command        | Parameters | Meaning          |
                // |----------------|------------|------------------|
                // | [Space][Space] | -          | Addition         |
                // | [Space][Tab]   | -          | Subtraction      |
                // | [Space][LF]    | -          | Multiplication   |
                // | [Tab][Space]   | -          | Integer Division |
                // | [Tab][Tab]     | -          | Modulo           |
            }
    
            
            /* Heap Access */
            else if (next_char == '\t')
            {
                // | Command | Parameters | Meaning  |
                // |---------|------------|----------|
                // | [Space] | -          | Store    |
                // | [Tab]   | -          | Retrieve |
            }
            
            
            /* Input/Output */
            else if (next_char == '\n')
            {
                // | Command        | Parameters | Meaning                                                                     |
                // |----------------|------------|-----------------------------------------------------------------------------|
                // | [Space][Space] | -          | Output the character at the top of the stack                                |
                // | [Space][Tab]   | -          | Output the number at the top of the stack                                   |
                // | [Tab][Space]   | -          | Read a character and place it in the location given by the top of the stack |
                // | [Tab][Tab]     | -          | Read a number and place it in the location given by the top of the stack    |
            }
        }
        
        /* Flow Control */
        else if (next_char == '\n')
        {
            // | Command        | Parameters | Meaning                                                  |
            // |----------------|------------|----------------------------------------------------------|
            // | [Space][Space] | Label      | Mark a location in the program                           |
            // | [Space][Tab]   | Label      | Call a subroutine                                        |
            // | [Space][LF]    | Label      | Jump unconditionally to a label                          |
            // | [Tab][Space]   | Label      | Jump to a label if the top of the stack is zero          |
            // | [Tab][Tab]     | Label      | Jump to a label if the top of the stack is negative      |
            // | [Tab][LF]      | -          | End a subroutine and transfer control back to the caller |
            // | [LF][LF]       | -          | End the program                                          |
        }
    }
}

int ws_convert(char *file_name)
{
    FILE *source_file;
    char next_char;
    
    source_file = fopen(file_name, "r");
    while ((next_char = fgetc(source_file)) != EOF)
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

int ws_echo(char* file_name)
{
    FILE *source_file;
    char next_char;
    int  char_counter = 0;

    source_file = fopen(file_name, "r");
    while ((next_char = fgetc(source_file)) != EOF)
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

int main(int argc, char *argv[])
{

    /* Shell */
    if (argc == 1)
    {
        ws_shell();
    }

    /* Interpreter */
    else if (strcmp(argv[1], "run") == 0)
    {
        ws_interpret(argv[2]);
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