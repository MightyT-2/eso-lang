// Whitespace Interpreter
// If no arguments are passed, it should act as a shell, similar to JShell or just typing Python in the terminal

// TODO: different versions
// TODO: proper error handling

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

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
    char           character;
    struct strchar *next_character;
} strchar;

typedef struct instruction
{
    struct instruction *next_instruct;
    uint8_t            instruct_num;
    int32_t            int_param;
    uint32_t           label_param;
} instruction;

typedef struct label
{
    instruction  *instruct_location;
    struct label *next_label,
                 *previous_label;
    uint32_t     label_id;
} label;

typedef struct process
{
    instruction *first_instruction,
                *current_instruction;
    label       *first_label,
                *last_label,
                *current_label;
} process;

typedef struct heap_entry
{
    struct heap_entry *next_entry,
                      *prev_entry;
    int32_t           heap_value;
    uint32_t          entry_id;
} heap_entry;

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
    
    printf("%d ", next_char);

    return next_char;
}

uint32_t get_label(FILE *source_file)
{
    uint32_t return_label = 0;
    int      digit_count = 0;
    char     next_char;

    while ((next_char = find_next_token(source_file)) != '\n')
    {
        if (next_char == ' ')
        {
            return_label = return_label << 1;
            if (return_label != 0)
            {
                digit_count++;
            }
        }
        
        else if (next_char == '\t')
        {
            return_label = return_label << 1;
            return_label++;
            digit_count++;
        }
        
        else if (next_char == EOF)
        {
            printf("Exiting. Invalid label.\n");
            exit(1);
        }
        if (digit_count > 32)
        {
            printf("Exiting. Label too large.\n");
            exit(1);
        }
    }
    printf("%d ", return_label);
    
    return return_label;
}

int32_t get_int(FILE *source_file)
{
    int32_t return_int = 0;
    int     digit_count = 0;
    char    next_char,
            sign_char;
    
    sign_char = find_next_token(source_file);
    if (sign_char == '\n')
    {
        return 0;
    }
    if (sign_char == EOF)
    {
        printf("Exiting. No integer parameter.\n");
        exit(1);
    }
    
    while ((next_char = find_next_token(source_file)) != '\n')
    {
        if (next_char == ' ')
        {
            return_int = return_int << 1;
            if (return_int != 0)
            {
                digit_count++;
            }
        }
        
        else if (next_char == '\t')
        {
            return_int = return_int << 1;
            return_int++;
            digit_count++;
        }
        
        else if (next_char == EOF)
        {
            printf("Exiting. Invalid label.\n");
            exit(1);
        }
        if (digit_count > 32)
        {
            printf("Exiting. Label too large.\n");
            exit(1);
        }
    }

    if (sign_char == '\t')
    {
        return_int = -return_int;
    }

    printf("%d ", return_int);
    
    return return_int;
}

int ws_load(char *file_location, process *empty_process)
{
    // printf("Run a program from a file\n");
    FILE        *source_file;
    char        next_char;
    instruction *current_instruct = NULL,
                *previous_instruct = NULL;
    label       *current_label = NULL,
                *previous_label = NULL;
    int         is_label = 0;
    
    source_file = fopen(file_location, "r");
    while ((next_char = find_next_token(source_file)) != EOF)
    {
        if (current_instruct == NULL && current_label == NULL)
        {
            empty_process->first_instruction = (instruction *)calloc(1, sizeof(instruction));
            empty_process->first_label = (label *)calloc(1, sizeof(label));
            current_instruct = empty_process->first_instruction;
            current_label = empty_process->first_label;
        }
        else
        {
            if(is_label != 1)
            {
                if(is_label == 2)
                {
                    current_label->instruct_location = current_instruct;
                    current_label->next_label = (label *)calloc(1, sizeof(label));
                    current_label->next_label->previous_label = current_label;
                    previous_label = current_label;
                    current_label = current_label->next_label;
                    is_label = 0;
                }
                current_instruct->next_instruct = (instruction *)calloc(1, sizeof(instruction));
                previous_instruct = current_instruct;
                current_instruct = current_instruct->next_instruct;
            }
            else
            {
                is_label = 2;
            }
        }

        /* Stack Manipulation */
        if (next_char == ' ')
        {
            // 1  | [Space][Space] | Number | Push a number to the stack |
            if ((next_char = find_next_token(source_file)) == ' ')
            {
                current_instruct->instruct_num = 1;
                current_instruct->int_param = get_int(source_file);
                printf("Push a number to the stack\n");
            }
            
            else if (next_char == '\t')
            {
                // 2  | [Space][Tab][Space] | Number | Copy the nth item on the stack (given by the argument) onto the top of the stack (v0.3) |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 2;
                    current_instruct->int_param = get_int(source_file);
                    printf("Copy the nth item on the stack (given by the argument) onto the top of the stack\n");
                }
                
                // 3  | [Space][Tab][LF] | Number | Slide n items off the stack, keeping the top item (v0.3) |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 3;
                    current_instruct->int_param = get_int(source_file);
                    printf("Slide n items off the stack, keeping the top item\n");
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
            }
            
            else if (next_char == '\n')
            {
                // 4  | [Space][LF][Space] | - | Duplicate the top item on the stack |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 4;
                    printf("Duplicate the top item on the stack\n");
                }
                
                // 5  | [Space][LF][Tab] | - | Swap the top two items on the stack |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 5;
                    printf("Swap the top two items on the stack\n");
                }
                
                // 6  | [Space][LF][LF] | - | Discard the top item on the stack |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 6;
                    printf("Discard the top item on the stack\n");
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
            }
            
            else
            {
                printf("Exiting. Invalid instruction\n");
                exit(1);
            }
        }
        
        /* Tab */
        else if (next_char == '\t')
        {
            
            /* Arithmetic */
            if ((next_char = find_next_token(source_file)) == ' ')
            {
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    // 7  | [Tab][Space][Space][Space] | - | Addition |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        current_instruct->instruct_num = 7;
                        printf("Addition\n");
                    }
                    
                    // 8  | [Tab][Space][Space][Tab] | - | Subtraction |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 8;
                        printf("Subtraction\n");
                    }
                    
                    // 9  | [Tab][Space][Space][LF] | - | Multiplication |
                    else if (next_char == '\n')
                    {
                        current_instruct->instruct_num = 9;
                        printf("Multiplication\n");
                    }
                    
                    else
                    {
                        printf("Exiting. Invalid instruction\n");
                        exit(1);
                    }
                }
                
                else if (next_char == '\t')
                {
                    // 10 | [Tab][Space][Tab][Space] | - | Integer Division |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        current_instruct->instruct_num = 10;
                        printf("Integer Division\n");
                    }
                    
                    // 11 | [Tab][Space][Tab][Tab] | - | Modulo |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 11;
                        printf("Modulo\n");
                    }
                    
                    else
                    {
                        printf("Exiting. Invalid instruction\n");
                        exit(1);
                    }
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
            }
            
            /* Heap Access */
            else if (next_char == '\t')
            {
                // 12 | [Tab][Tab][Space] | - | Store |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 12;
                    printf("Store\n");
                    
                }
                
                // 13 | [Tab][Tab][Tab] | - | Retrieve |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 13;
                    printf("Retrieve\n");
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
            }
            
            
            /* Input/Output */
            else if (next_char == '\n')
            {
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    // 14 | [Tab][LF][Space][Space] | - | Output the character at the top of the stack |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        current_instruct->instruct_num = 14;
                        printf("Output the character at the top of the stack\n");
                    }
                    
                    // 15 | [Tab][LF][Space][Tab] | - | Output the number at the top of the stack |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 15;
                        printf("Output the number at the top of the stack\n");
                    }
                    
                    else
                    {
                        printf("Exiting. Invalid instruction\n");
                        exit(1);
                    }
                }
                
                else if (next_char == '\t')
                {
                    // 16 | [Tab][LF][Tab][Space] | - | Read a character and place it in the location given by the top of the stack |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        current_instruct->instruct_num = 16;
                        printf("Read a character and place it in the location given by the top of the stack\n");
                    }
                    
                    // 17 | [Tab][LF][Tab][Tab] | - | Read a number and place it in the location given by the top of the stack |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 17;
                        printf("Read a number and place it in the location given by the top of the stack\n");
                    }
                    
                    else
                    {
                        printf("Exiting. Invalid instruction\n");
                        exit(1);
                    }
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
            }
            
            else
            {
                printf("Exiting. Invalid instruction\n");
                exit(1);
            }
        }
        
        /* Flow Control */
        else if (next_char == '\n')
        {
            if ((next_char = find_next_token(source_file)) == ' ')
            {
                //    | [LF][Space][Space] | Label | Mark a location in the program |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_label->label_id = get_label(source_file);
                    is_label = 1;
                    printf("Mark a location in the program\n");
                }
                
                // 18 | [LF][Space][Tab] | Label | Call a subroutine |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 18;
                    current_instruct->label_param = get_label(source_file);
                    printf("Call a subroutine\n");
                }
                
                // 19 | [LF][Space][LF] | Label | Jump unconditionally to a label |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 19;
                    current_instruct->label_param = get_label(source_file);
                    printf("Jump unconditionally to a label\n");
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
                
            }
            
            else if (next_char == '\t')
            {
                // 20 | [LF][Tab][Space] | Label | Jump to a label if the top of the stack is zero |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 20;
                    current_instruct->label_param = get_label(source_file);
                    printf("Jump to a label if the top of the stack is zero\n");
                }
                
                // 21 | [LF][Tab][Tab] | Label | Jump to a label if the top of the stack is negative |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 21;
                    current_instruct->label_param = get_label(source_file);
                    printf("Jump to a label if the top of the stack is negative\n");
                }
                
                // 22 | [LF][Tab][LF] | - | End a subroutine and transfer control back to the caller |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 22;
                    current_instruct->label_param = get_label(source_file);
                    printf("End a subroutine and transfer control back to the caller\n");
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
                
            }
            
            else if (next_char == '\n')
            {
                // 23 | [LF][LF][LF] | - | End the program |
                if ((next_char = find_next_token(source_file)) == '\n')
                {
                    current_instruct->instruct_num = 23;
                    printf("End the program\n");
                }
                
                else
                {
                    printf("Exiting. Invalid instruction\n");
                    exit(1);
                }
            }
            
            else
            {
                printf("Exiting. Invalid instruction\n");
                exit(1);
            }
        }
        
        else
        {
            printf("Exiting. Invalid instruction\n");
            exit(1);
        }
    }
    if (previous_instruct == NULL)
    {
        printf("Exiting. No instructions have been generated.\n");
        exit(1);
    }
    
    if (is_label == 2)
    {
        printf("Exiting. Label pointing past instructions.\n");
    }

    
    return 0;
}

int ws_run(process running_process)
{
    int32_t     *stack_base,
                *stack_entry;
    instruction **call_stack_base,
                **call_stack_entry;
    heap_entry  *first_heap_entry = NULL,
                *current_heap_entry = NULL;
    int32_t     register_1,
                register_2;

    if ((stack_base = (int32_t *)malloc(1024 * sizeof(int32_t))) == NULL)
    {
        printf("Exiting. Not enough memory for stack allocation\n");
        exit(1);
    }
    stack_entry = stack_base;
    if ((call_stack_base = (instruction **)malloc(1024 * sizeof(instruction **))) == NULL)
    {
        printf("Exiting. Not enough memory for call stack allocation\n");
        exit(1);
    }
    call_stack_entry = call_stack_base;

    // 23 | [LF][LF][LF] | - | End the program |
    while (running_process.current_instruction->instruct_num != 23)
    {
        // 1  | [Space][Space] | Number | Push a number to the stack |
        if (running_process.current_instruction->instruct_num == 1)
        {
            stack_entry--;
            if (stack_entry < stack_base)
            {
                printf("Exiting. Stack underflow.\n");
                exit(1);
            }
            register_1 = *stack_entry;
            stack_entry--;
            if (stack_entry < stack_base)
            {
                printf("Exiting. Stack underflow.\n");
                exit(1);
            }
            register_2 = *stack_entry;
            *stack_entry = register_1 + register_2;
            stack_entry++;
        }
        // 2  | [Space][Tab][Space]        | Number     | Copy the nth item on the stack (given by the argument) onto the top of the stack (v0.3) |
        else if (running_process.current_instruction->instruct_num == 2)
        {
        }
        // 3  | [Space][Tab][LF]           | Number     | Slide n items off the stack, keeping the top item (v0.3)                                |
        else if (running_process.current_instruction->instruct_num == 3)
        {
        }
        // 4  | [Space][LF][Space]         | -          | Duplicate the top item on the stack                                                     |
        else if (running_process.current_instruction->instruct_num == 4)
        {
        }
        // 5  | [Space][LF][Tab]           | -          | Swap the top two items on the stack                                                     |
        else if (running_process.current_instruction->instruct_num == 5)
        {
        }
        // 6  | [Space][LF][LF]            | -          | Discard the top item on the stack                                                       |
        else if (running_process.current_instruction->instruct_num == 6)
        {
        }
        // 7  | [Tab][Space][Space][Space] | -          | Addition                                                                                |
        else if (running_process.current_instruction->instruct_num == 7)
        {
        }
        // 8  | [Tab][Space][Space][Tab]   | -          | Subtraction                                                                             |
        else if (running_process.current_instruction->instruct_num == 8)
        {
        }
        // 9  | [Tab][Space][Space][LF]    | -          | Multiplication                                                                          |
        else if (running_process.current_instruction->instruct_num == 9)
        {
        }
        // 10 | [Tab][Space][Tab][Space]   | -          | Integer Division                                                                        |
        else if (running_process.current_instruction->instruct_num == 10)
        {
        }
        // 11 | [Tab][Space][Tab][Tab]     | -          | Modulo                                                                                  |
        else if (running_process.current_instruction->instruct_num == 11)
        {
        }
        // 12 | [Tab][Tab][Space]          | -          | Store                                                                                   |
        else if (running_process.current_instruction->instruct_num == 12)
        {
        }
        // 13 | [Tab][Tab][Tab]            | -          | Retrieve                                                                                |
        else if (running_process.current_instruction->instruct_num == 13)
        {
        }
        // 14 | [Tab][LF][Space][Space]    | -          | Output the character at the top of the stack                                            |
        else if (running_process.current_instruction->instruct_num == 14)
        {
        }
        // 15 | [Tab][LF][Space][Tab]      | -          | Output the number at the top of the stack                                               |
        else if (running_process.current_instruction->instruct_num == 15)
        {
        }
        // 16 | [Tab][LF][Tab][Space]      | -          | Read a character and place it in the location given by the top of the stack             |
        else if (running_process.current_instruction->instruct_num == 16)
        {
        }
        // 17 | [Tab][LF][Tab][Tab]        | -          | Read a number and place it in the location given by the top of the stack                |
        else if (running_process.current_instruction->instruct_num == 17)
        {
        }
        // 18 | [LF][Space][Tab]           | Label      | Call a subroutine                                                                       |
        else if (running_process.current_instruction->instruct_num == 18)
        {
        }
        // 19 | [LF][Space][LF]            | Label      | Jump unconditionally to a label                                                         |
        else if (running_process.current_instruction->instruct_num == 19)
        {
        }
        // 20 | [LF][Tab][Space]           | Label      | Jump to a label if the top of the stack is zero                                         |
        else if (running_process.current_instruction->instruct_num == 20)
        {
        }
        // 21 | [LF][Tab][Tab]             | Label      | Jump to a label if the top of the stack is negative                                     |
        else if (running_process.current_instruction->instruct_num == 21)
        {
        }
        // 22 | [LF][Tab][LF]              | -          | End a subroutine and transfer control back to the caller                                |
        else if (running_process.current_instruction->instruct_num == 22)
        {
        }
        else
        {
            printf("Exiting. Invalid Instruction");
            exit(1);
        }

        if (running_process.current_instruction->next_instruct == NULL)
        {
            printf("Exiting. Terminating program with no end instruction.\n");
            exit(1);
        }
        running_process.current_instruction = running_process.current_instruction->next_instruct;
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
    process user_process;
    int32_t test = -32;

    /* Shell */
    if (argc == 1)
    {
        ws_shell();
    }

    /* Interpreter */
    else if (strcmp(argv[1], "run") == 0)
    {
        printf("Results: %d\n", ws_load(argv[2], &user_process));
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
//    | [LF][Space][Space]         | Label      | Mark a location in the program                                                          |
// 18 | [LF][Space][Tab]           | Label      | Call a subroutine                                                                       |
// 19 | [LF][Space][LF]            | Label      | Jump unconditionally to a label                                                         |
// 20 | [LF][Tab][Space]           | Label      | Jump to a label if the top of the stack is zero                                         |
// 21 | [LF][Tab][Tab]             | Label      | Jump to a label if the top of the stack is negative                                     |
// 22 | [LF][Tab][LF]              | -          | End a subroutine and transfer control back to the caller                                |
// 23 | [LF][LF][LF]               | -          | End the program                                                                         |