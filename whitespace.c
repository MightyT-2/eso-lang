// Whitespace Interpreter
// If no arguments are passed, it should act as a shell, similar to JShell or just typing Python in the terminal

// TODO: different versions

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>

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
    instruction *next_instruct;
    uint8_t     instruct_num;
    int32_t     int_param;
    uint32_t    label_param;
} instruction;

typedef struct label
{
    instruction *label_location;
    label       *next_label,
                *previous_label;
} label;

typedef struct process
{
    instruction *first_instruction,
                *current_instruction;
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

int ws_load(char *file_location, process *empty_process)
{
    // printf("Run a program from a file\n");
    FILE     *source_file;
    char     next_char;
    uint8_t  instruct_value;
    uint32_t label_param;
    int32_t  int_param;

    source_file = fopen(file_location, "r");
    while ((next_char = find_next_token(source_file)) != EOF)
    {


        /* Stack Manipulation */
        if (next_char == ' ')
        {
            // 1  | [Space][Space]             | Number     | Push a number to the stack                                                              |
            if (next_char == ' ')
            {

            }
            
            else if (next_char == ' \t')
            {
                // 2  | [Space][Tab][Space]        | Number     | Copy the nth item on the stack (given by the argument) onto the top of the stack (v0.3) |
                if (next_char == ' ')
                {

                }
                
                // 3  | [Space][Tab][LF]           | Number     | Slide n items off the stack, keeping the top item (v0.3)                                |
                else if (next_char == '\n')
                {
                    
                }

                else
                {

                }
            }

            else if (next_char == '\n')
            {
                // 4  | [Space][LF][Space]         | -          | Duplicate the top item on the stack                                                     |
                if (next_char == ' ')
                {
                    
                }
                
                // 5  | [Space][LF][Tab]           | -          | Swap the top two items on the stack                                                     |
                else if (next_char == '\t')
                {

                }
                
                // 6  | [Space][LF][LF]            | -          | Discard the top item on the stack                                                       |
                else if (next_char == '\n')
                {

                }

                else
                {
                    
                }

            }

            else
            {
                
            }
        }
        
        /* Tab */
        else if (next_char == '\t')
        {
            
            /* Arithmetic */
            if (next_char == ' ')
            {
                if (next_char == ' ')
                {
                    // 7  | [Tab][Space][Space][Space] | -          | Addition                                                                                |
                    if (next_char == ' ')
                    {

                    }
                    
                    // 8  | [Tab][Space][Space][Tab]   | -          | Subtraction                                                                             |
                    else if (next_char == '\t')
                    {
                        
                    }
                    
                    // 9  | [Tab][Space][Space][LF]    | -          | Multiplication                                                                          |
                    else if (next_char == '\n')
                    {
                        
                    }
                    
                    else
                    {
                        
                    }

                }
                
                else if (next_char == '\t')
                {
                    // 10 | [Tab][Space][Tab][Space]   | -          | Integer Division                                                                        |
                    if (next_char == ' ')
                    {

                    }

                    // 11 | [Tab][Space][Tab][Tab]     | -          | Modulo                                                                                  |
                    else if (next_char == '\t')
                    {

                    }

                    else
                    {
                        
                    }

                }

                else
                {

                }
            }
    
            
            /* Heap Access */
            else if (next_char == '\t')
            {
                // 12 | [Tab][Tab][Space]          | -          | Store                                                                                   |
                if (next_char == ' ')
                {

                }

                // 13 | [Tab][Tab][Tab]            | -          | Retrieve                                                                                |
                else if (next_char == '\t')
                {

                }

                else
                {
                    
                }
            }
            
            
            /* Input/Output */
            else if (next_char == '\n')
            {
                if (next_char == ' ')
                {
                    // 14 | [Tab][LF][Space][Space]    | -          | Output the character at the top of the stack                                            |
                    if (next_char == ' ')
                    {

                    }

                    // 15 | [Tab][LF][Space][Tab]      | -          | Output the number at the top of the stack                                               |
                    else if (next_char == '\t')
                    {

                    }

                    else
                    {

                    }

                }
                
                else if (next_char == '\t')
                {
                    // 16 | [Tab][LF][Tab][Space]      | -          | Read a character and place it in the location given by the top of the stack             |
                    if (next_char == ' ')
                    {

                    }

                    // 17 | [Tab][LF][Tab][Tab]        | -          | Read a number and place it in the location given by the top of the stack                |
                    else if (next_char == '\t')
                    {

                    }

                    else
                    {

                    }

                }

                else
                {

                }
            }

            else
            {
                
            }
        }
        
        /* Flow Control */
        else if (next_char == '\n')
        {
            if (next_char == ' ')
            {
                // 18 | [LF][Space][Space]         | Label      | Mark a location in the program                                                          |
                if (next_char == ' ')
                {

                }

                // 19 | [LF][Space][Tab]           | Label      | Call a subroutine                                                                       |
                else if (next_char == '\t')
                {
                    
                }
                
                // 20 | [LF][Space][LF]            | Label      | Jump unconditionally to a label                                                         |
                else if (next_char == '\n')
                {

                }

                else
                {

                }

            }

            else if (next_char == '\t')
            {
                // 21 | [LF][Tab][Space]           | Label      | Jump to a label if the top of the stack is zero                                         |
                if (next_char == ' ')
                {
                    
                }
                
                // 22 | [LF][Tab][Tab]             | Label      | Jump to a label if the top of the stack is negative                                     |
                else if (next_char == '\t')
                {

                }

                // 23 | [LF][Tab][LF]              | -          | End a subroutine and transfer control back to the caller                                |
                else if (next_char == '\n')
                {

                }

                else
                {
                    
                }

            }

            else if (next_char == '\n')
            {
                // 24 | [LF][LF][LF]               | -          | End the program                                                                         |
                if (next_char == '\n')
                {

                }

                else
                {

                }

            }

            else
            {

            }
        }

        else
        {
            
        }
    }

    return 0;
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
    process user_process;

    /* Shell */
    if (argc == 1)
    {
        ws_shell();
    }

    /* Interpreter */
    else if (strcmp(argv[1], "run") == 0)
    {
        ws_load(argv[2], &user_process);
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

//    | Command                    | Parameters | Meaning                                                                                 |
//    |----------------------------|------------|-----------------------------------------------------------------------------------------|
// 1  | [Space][Space]             | Number     | Push a number to the stack                                                              |
// 2  | [Space][Tab][Space]        | Number     | Copy the nth item on the stack (given by the argument) onto the top of the stack (v0.3) |
// 3  | [Space][Tab][LF]           | Number     | Slide n items off the stack, keeping the top item (v0.3)                                |
// 4  | [Space][LF][Space]         | -          | Duplicate the top item on the stack                                                     |
// 5  | [Space][LF][Tab]           | -          | Swap the top two items on the stack                                                     |
// 6  | [Space][LF][LF]            | -          | Discard the top item on the stack                                                       |
// 7  | [Tab][Space][Space][Space] | -          | Addition                                                                                |
// 8  | [Tab][Space][Space][Tab]   | -          | Subtraction                                                                             |
// 9  | [Tab][Space][Space][LF]    | -          | Multiplication                                                                          |
// 10 | [Tab][Space][Tab][Space]   | -          | Integer Division                                                                        |
// 11 | [Tab][Space][Tab][Tab]     | -          | Modulo                                                                                  |
// 12 | [Tab][Tab][Space]          | -          | Store                                                                                   |
// 13 | [Tab][Tab][Tab]            | -          | Retrieve                                                                                |
// 14 | [Tab][LF][Space][Space]    | -          | Output the character at the top of the stack                                            |
// 15 | [Tab][LF][Space][Tab]      | -          | Output the number at the top of the stack                                               |
// 16 | [Tab][LF][Tab][Space]      | -          | Read a character and place it in the location given by the top of the stack             |
// 17 | [Tab][LF][Tab][Tab]        | -          | Read a number and place it in the location given by the top of the stack                |
// 18 | [LF][Space][Space]         | Label      | Mark a location in the program                                                          |
// 19 | [LF][Space][Tab]           | Label      | Call a subroutine                                                                       |
// 20 | [LF][Space][LF]            | Label      | Jump unconditionally to a label                                                         |
// 21 | [LF][Tab][Space]           | Label      | Jump to a label if the top of the stack is zero                                         |
// 22 | [LF][Tab][Tab]             | Label      | Jump to a label if the top of the stack is negative                                     |
// 23 | [LF][Tab][LF]              | -          | End a subroutine and transfer control back to the caller                                |
// 24 | [LF][LF][LF]               | -          | End the program                                                                         |