// Whitespace Interpreter
// If no arguments are passed, it should act as a shell, similar to JShell or just typing Python in the terminal

// TODO: shell
// TODO: proper error handling
// TODO: memory management

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#ifndef _STACK
#include "stack.h"
#endif
#ifndef _HEAP
#include "heap.h"
#endif
#ifndef _WHITESPACE
#include "whitespace.h"
#endif
#ifndef _INSTRUCTION
#include "instruction.h"
#endif

#define NOCOLOR                  "\x1b[0m"
#define RED_FORE                 "\x1b[31m"
#define GREEN_FORE               "\x1b[32m"
#define YELLOW_FORE              "\x1b[33m"
#define BLUE_FORE                "\x1b[34m"
#define MAGENTA_FORE             "\x1b[35m"
#define CYAN_FORE                "\x1b[36m"
#define RED_BACK                 "\x1b[41m"
#define GREEN_BACK               "\x1b[42m"
#define YELLOW_BACK              "\x1b[43m"
#define BLUE_BACK                "\x1b[44m"
#define MAGENTA_BACK             "\x1b[45m"
#define CYAN_BACK                "\x1b[46m"
#define ERR_STACK_OVERFLOW       "Exiting. Stack overflow.\r\n"
#define ERR_STACK_UNDERFLOW      "Exiting. Stack underflow.\r\n"
#define ERR_CALL_STACK_OVERFLOW  "Exiting. Call stack overflow.\r\n"
#define ERR_CALL_STACK_UNDERFLOW "Exiting. Call stack underflow.\r\n"
#define ERR_INVAL_INSTRUCT       "Exiting. Invalid instruction.\r\n"
#define ERR_DIV_ZERO             "Exiting. Divide by zero.\r\n"
#define ERR_NOT_NUMBER           "Exiting. Not a number.\r\n"
#define ERR_NO_INTEGER           "Exiting. No integer parameter.\r\n"
#define HERE                     "here\r\n"
#define STD_OUTPUT               1

typedef struct strchar
{
    char           character;
    struct strchar *next_character;
} strchar;

int exception = 0;

/* Debug function used to step through each instruction */
void debug_step()
{
    char empty[1];

    scanf("%1c", empty);
}

/* Searches a file pointer for the next whitespace character */
char find_next_token(FILE *source_file)
{
    char next_char;

    /* Loop until a valid character is found */
    do
    {
        next_char = fgetc(source_file);
        // if (next_char == ' ')
        // {
        //     printf("[Space]    ");
        // }
        // else if (next_char == '\t')
        // {
        //     printf("[Tab]      ");
        // }
        // else if (next_char == '\n')
        // {
        //     printf("[Line Feed]");
        // }
        // printf("Token Found: %d\r\n", next_char);
    } while(next_char != ' ' && next_char != '\t' && next_char != '\n' && next_char != EOF);
    
    return next_char;
}

/* Converts whitespace bits to a 64 bit integer */
int64_t get_int(FILE *source_file)
{
    int64_t return_int = 0;
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
        exception = 1;
        return 1;
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
            printf("Exiting. Invalid integer.\n");
            exception = 1;
            return 1;
        }
        if (digit_count > 64)
        {
            printf("Exiting. Integer too large.\n");
            exception = 1;
            return 1;
        }
    }
    
    if (sign_char == '\t')
    {
        return_int = -return_int;
    }

    return return_int;
}

/* Converts whitespace bits to an instructor label */
/* TODO: Use realloc() */
char *get_label(FILE *source_file)
{
    char    *return_label,
            *label_entry,
            next_char;
    int     char_count    = 0;
    strchar *label_holder = NULL,
            *curr_char    = NULL;

    /* Loop until a line feed is enountered */
    while ((next_char = find_next_token(source_file)) != '\n')
    {

        /* Initialize the label */
        if (label_holder == NULL)
        {
            if ((label_holder = (strchar *)calloc(1, sizeof(strchar))) == NULL)
            {
                printf("Exiting. Allocation error.\n");
                return NULL;
            }
            curr_char = label_holder;
        }

        /* Initialize the next character in the label */
        else
        {
            curr_char->next_character = (strchar *)calloc(1, sizeof(strchar));
            curr_char = curr_char->next_character;
        }

        /* Add the next character to the label */
        if (next_char == ' ' || next_char == '\t')
        {
            curr_char->character = next_char;
        }

        /* Exit if the next character is invalid */
        else if (next_char == EOF)
        {
            printf("Exiting. Invalid label.\n");
            return NULL;
        }
        char_count++;
    }

    /* Initialize the return label */
    return_label = (char *)calloc(char_count + 1, sizeof(char));
    label_entry = return_label;
    
    /* Add characters to the label */
    for (curr_char = label_holder; curr_char->next_character != NULL; curr_char = curr_char->next_character)
    {
        *label_entry = curr_char->character;
        label_entry++;
    }
    
    return return_label;
}

/* Convert the whitespace source code to a legible format */
void ws_convert(char *file_name)
{
    FILE *source_file;
    char next_char;
    
    /* Print the program in single character notation */
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

    return;
}

/* Echos the whitespace source code with character coloring */
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

    return 0;
}

/* Loads the instructions into a process */
int ws_load(FILE *source_file)
{
    char        next_char;
    
    /* Loop until an end of file character is encountered */
    while ((next_char = find_next_token(source_file)) != EOF)
    {
        
        /* Stack Manipulation */
        if (next_char == ' ')
        {
            // 1  | [Space][Space] | Number | Push a number to the stack |
            if ((next_char = find_next_token(source_file)) == ' ')
            {
                add_instruction(instruct_push, get_int(source_file), NULL, 0, 0);
            }
            
            else if (next_char == '\t')
            {
                // 2  | [Space][Tab][Space] | Number | Copy the nth item on the stack (given by the argument) onto the top of the stack (v0.3) |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    add_instruction(instruct_copy, get_int(source_file), NULL, 0, 0);
                }
                
                // 3  | [Space][Tab][LF] | Number | Slide n items off the stack, keeping the top item (v0.3) |
                else if (next_char == '\n')
                {
                    add_instruction(instruct_slide, get_int(source_file), NULL, 0, 0);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("stack\n");
                    return 1;
                }
            }
            
            else if (next_char == '\n')
            {
                // 4  | [Space][LF][Space] | - | Duplicate the top item on the stack |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    add_instruction(instruct_dup, 0, NULL, 0, 0);
                }
                
                // 5  | [Space][LF][Tab] | - | Swap the top two items on the stack |
                else if (next_char == '\t')
                {
                    add_instruction(instruct_swap, 0, NULL, 0, 0);
                }
                
                // 6  | [Space][LF][LF] | - | Discard the top item on the stack |
                else if (next_char == '\n')
                {
                    add_instruction(instruct_discard, 0, NULL, 0, 0);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("top of stack\n");
                    return 1;
                }
            }
            
            else
            {
                printf(ERR_INVAL_INSTRUCT);
                printf("stack 2\n");
                return 1;
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
                        add_instruction(instruct_add, 0, NULL, 0, 0);
                    }
                    
                    // 8  | [Tab][Space][Space][Tab] | - | Subtraction |
                    else if (next_char == '\t')
                    {
                        add_instruction(instruct_sub, 0, NULL, 0, 0);
                    }
                    
                    // 9  | [Tab][Space][Space][LF] | - | Multiplication |
                    else if (next_char == '\n')
                    {
                        add_instruction(instruct_multi, 0, NULL, 0, 0);
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        printf("arithmetic\n");
                        return 1;
                    }
                }
                
                else if (next_char == '\t')
                {
                    // 10 | [Tab][Space][Tab][Space] | - | Integer Division |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        add_instruction(instruct_div, 0, NULL, 0, 0);
                    }
                    
                    // 11 | [Tab][Space][Tab][Tab] | - | Modulo |
                    else if (next_char == '\t')
                    {
                        add_instruction(instruct_mod, 0, NULL, 0, 0);
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        printf("more arithmetic\n");
                        return 1;
                    }
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("more aritmetic again\n");
                    return 1;
                }
            }
            
            /* Heap Access */
            else if (next_char == '\t')
            {
                // 12 | [Tab][Tab][Space] | - | Store |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    add_instruction(instruct_store, 0, NULL, 0, 0);
                }
                
                // 13 | [Tab][Tab][Tab] | - | Retrieve |
                else if (next_char == '\t')
                {
                    add_instruction(instruct_fetch, 0, NULL, 0, 0);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("heap access\n");
                    return 1;
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
                        add_instruction(instruct_output_c, 0, NULL, 0, 0);
                    }
                    
                    // 15 | [Tab][LF][Space][Tab] | - | Output the number at the top of the stack |
                    else if (next_char == '\t')
                    {
                        add_instruction(instruct_output_i, 0, NULL, 0, 0);
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        printf("output\n");
                        return 1;
                    }
                }
                
                else if (next_char == '\t')
                {
                    // 16 | [Tab][LF][Tab][Space] | - | Read a character and place it in the location given by the top of the stack |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        add_instruction(instruct_input_c, 0, NULL, 0, 0);
                    }
                    
                    // 17 | [Tab][LF][Tab][Tab] | - | Read a number and place it in the location given by the top of the stack |
                    else if (next_char == '\t')
                    {
                        add_instruction(instruct_input_i, 0, NULL, 0, 0);
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        printf("input\n");
                        return 1;
                    }
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("in out\n");
                    return 1;
                }
            }
            
            else
            {
                printf(ERR_INVAL_INSTRUCT);
                printf("Tab options\n");
                return 1;
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
                    add_label(get_label(source_file));
                }
                
                // 18 | [LF][Space][Tab] | Label | Call a subroutine |
                else if (next_char == '\t')
                {
                    add_instruction(instruct_call, 0, get_label(source_file), 0, 0);
                }
                
                // 19 | [LF][Space][LF] | Label | Jump unconditionally to a label |
                else if (next_char == '\n')
                {
                    add_instruction(instruct_jump, 0, get_label(source_file), 0, 0);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("jumps\n");
                    return 1;
                }
                
            }
            
            else if (next_char == '\t')
            {
                // 20 | [LF][Tab][Space] | Label | Jump to a label if the top of the stack is zero |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    add_instruction(instruct_jump_zero, 0, get_label(source_file), 0, 0);
                }
                
                // 21 | [LF][Tab][Tab] | Label | Jump to a label if the top of the stack is negative |
                else if (next_char == '\t')
                {
                    add_instruction(instruct_jump_neg, 0, get_label(source_file), 0, 0);
                }
                
                // 22 | [LF][Tab][LF] | - | End a subroutine and transfer control back to the caller |
                else if (next_char == '\n')
                {
                    add_instruction(instruct_return, 0, NULL, 0, 0);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("subroutines\n");
                    return 1;
                }
                
            }
            
            else if (next_char == '\n')
            {
                // 23 | [LF][LF][LF] | - | End the program |
                if ((next_char = find_next_token(source_file)) == '\n')
                {
                    add_instruction(instruct_end, 0, NULL, 0, 0);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    printf("end\n");
                    return 1;
                }
            }
            
            else
            {
                printf(ERR_INVAL_INSTRUCT);
                printf("flow control\n");
                return 1;
            }
        }
        
        else
        {
            printf(ERR_INVAL_INSTRUCT);
            printf("everything\n");
            return 1;
        }
    }

    connect_labels();
    
    return 0;
}

/* Run a whitespace process */
int ws_run()
{
    int64_t param,
            item_1,
            item_2,
            item_3;
    int     is_error,
            instruction,
            line,
            column;
    int     debug_int = 0;
    
    log_error(start_program(), 0, 0);

    /* Loop until an end process instruction is reached */
    while (count_error() == 0)
    {
        switch (get_instruct(&instruction, &param, &line, &column), instruction)
        {
            
            /* Push a number to the stack */
            case instruct_push:
                log_error(push(param), line, column);
                log_error(next_instruct(0), line, column);
                break;
                
                /* Copy the nth item on the stack */
            case instruct_copy:
                log_error(peek(param, &item_1), line, column);
                log_error(push(item_1), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Slide n items of the stack while keeping the top item */
            case instruct_slide:
                for (item_1 = param; item_1 > 0; item_1--)
                {
                    log_error(pop(NULL), line, column);
                }
                log_error(next_instruct(0), line, column);
                break;

            /* Duplicate the top stack item */
            case instruct_dup:
                log_error(peek(0, &item_1), line, column);
                log_error(push(item_1), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Swap the top two items on the stack */
            case instruct_swap:
                log_error(pop(&item_1), line, column);
                log_error(pop(&item_2), line, column);
                log_error(push(item_1), line, column);
                log_error(push(item_2), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Discard the top item off the stack */
            case instruct_discard:
                log_error(pop(NULL), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Perform addition with the top two items on the stack */
            case instruct_add:
                log_error(pop(&item_1), line, column);
                log_error(pop(&item_2), line, column);
                log_error(push(item_2 + item_1), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Perform subtraction with the top two items on the stack */
            case instruct_sub:
                log_error(pop(&item_1), line, column);
                log_error(pop(&item_2), line, column);
                log_error(push(item_2 - item_1), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Perform multiplication with the top two items on the stack */
            case instruct_multi:
                log_error(pop(&item_1), line, column);
                log_error(pop(&item_2), line, column);
                log_error(push(item_2 * item_1), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Perform integer division with the top two items on the stack */
            case instruct_div:
                log_error(pop(&item_1), line, column);
                log_error(pop(&item_2), line, column);
                log_error(push(item_2 * item_1), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Perform modulo with the top two items on the stack */
            case instruct_mod:
                log_error(pop(&item_1), line, column);
                log_error(pop(&item_2), line, column);
                log_error(push(item_2 % item_1), line, column);
                log_error(next_instruct(0), line, column);
                break;

            /* Store the top item on the stack in a heap entry */
            case instruct_store:
                log_error(pop(&item_1), line, column);
                log_error(pop(&item_2), line, column);
                store(item_2, item_1);
                log_error(next_instruct(0), line, column);
                break;

            /* Retrieve an item from a heap entry */
            case instruct_fetch:
                log_error(pop(&item_1), line, column);
                log_error(fetch(item_1, &item_2), line, column);
                log_error(push(item_2), line, column);
                log_error(next_instruct(0), line, column);
                break;
            
            /* Output the character at the top of the stack */
            case instruct_output_c:
                log_error(pop(&item_1), line, column);
                printf("%lc", (wchar_t) item_1);
                log_error(next_instruct(0), line, column);
                break;
            
            /* Output the integer at the top of the stack */
            case instruct_output_i:
                log_error(pop(&item_1), line, column);
                printf("%ld", item_1);
                log_error(next_instruct(0), line, column);
                break;
            
            /* Read a character and place it in a heap entry */
            case instruct_input_c:
                scanf("%1lc", (wchar_t *)&item_1);
                log_error(pop(&item_2), line, column);
                store(item_2, item_1);
                log_error(next_instruct(0), line, column);
                break;
            
            /* Read an integer and place it in a heap entry */
            case instruct_input_i:
                item_3 = 0;
                scanf("%lc", (int *)&item_1);
                if (item_1 == '-')
                {
                    item_2 = 1;
                    scanf("%lc", (int *)&item_1);
                }
                else
                {
                    item_2 = 0;
                }
                while (item_1 != '\n')
                {
                    if (item_1 < '0' || item_1 > '9')
                    {
                        printf(ERR_NOT_NUMBER);
                        return 1;
                    }
                    item_3 *= 10;
                    item_3 += item_1 - '0';
                    scanf("%lc", (int *)&item_1);
                }
                if (item_2)
                {
                    item_3 = -item_3;
                }
                log_error(pop(&item_1), line, column);
                store(item_1, item_3);
                log_error(next_instruct(0), line, column);
                break;
        
            /* Call a subroutine */
            case instruct_call:
                log_error(enter_subroutine(), line, column);
                break;
        
            /* Jump to a label unconditionally */
            case instruct_jump:
                log_error(next_instruct(1), line, column);
                break;
            
            /* Jump to a label if the top of the stack is zero */
            case instruct_jump_zero:
                log_error(pop(&item_1), line, column);
                log_error(next_instruct(item_1 == 0), line, column);
                break;
            
            /* Jump to a label if the top of the stack is negative */
            case instruct_jump_neg:
                log_error(pop(&item_1), line, column);
                log_error(next_instruct(item_1 < 0), line, column);
                break;
            
            /* Return from a subroutine */
            case instruct_return:
                log_error(return_subroutine(), line, column);
                break;
            
            case instruct_end:
                return 0;
            
            /* End running if there is no end program instruction */
            default:
                return 1;
        }

        /* Debug Messages */
        // printf("%d\n", debug_int++);
        // printf("Current instruction: %2d, Int param: %ld\n", instruction, param);
        // stack_dump();
        // heap_dump();
        // debug_step();
    }

    return 1;
}

void print_char(char output, int *curr_col, int *curr_row, int *term_col, int *term_row)
{
    int iterator;
    
    if (output >= 33 && output <= 126)
    {
        printf("%c", output);
        *curr_col += 1;
        if (*curr_col >= *term_col)
        {
            *curr_col = 0;
            *curr_row += 1;
        }
    }
    else if (output == 32)
    {
        printf(GREEN_BACK " " NOCOLOR);
    }
    else if (output == 9)
    {
        8 - (*curr_col % 8);
        if (*curr_col + 8 - (*curr_col % 8) >= *term_col)
        {
            printf("\r\n");
            *curr_col = 0;
            *curr_row += 1;
        }
        do
        {
            printf(BLUE_BACK " " NOCOLOR);
            *curr_col += 1;
        } while (*curr_col % 8 != 0);
    }
    else if (output == '\n')
    {
        printf(RED_BACK " " NOCOLOR);
        *curr_col += 1;
        if (*curr_col >= *term_col)
        {
            *curr_row += 1;
        }
        printf("\r\n");
        *curr_col = 0;
        *curr_row += 1;
    }

    fflush(stdout);

    return;
}

void print_help()
{
    printf("A Whitespace Interpreter\n");
    printf("\n");
    printf("Run a Program:\n");
    printf("    whitespace run (file_name.ws)\n");
    printf("\n");
    printf("Echo Source Code:\n");
    printf("    whitespace echo (file_name.ws)\n");
    printf("\n");
    printf("Convert Source Code:\n");
    printf("    whitespace cws (file)name.ws)\n");
    printf("\n");

    return;
}

int calc_line_len();

//    | Command                    | Parameters | Meaning                                                                                 |
//    |----------------------------|------------|-----------------------------------------------------------------------------------------|
// 0  | [Space][Space]             | Number     | Push a number to the stack                                                              |
// 1  | [Space][Tab][Space]        | Number     | Copy the nth item on the stack (given by the argument) onto the top of the stack (v0.3) |
// 2  | [Space][Tab][LF]           | Number     | Slide n items off the stack, keeping the top item (v0.3)                                |
// 3  | [Space][LF][Space]         | -          | Duplicate the top item on the stack                                                     |
// 4  | [Space][LF][Tab]           | -          | Swap the top two items on the stack                                                     |
// 5  | [Space][LF][LF]            | -          | Discard the top item on the stack                                                       |
// 6  | [Tab][Space][Space][Space] | -          | Addition                                                                                |
// 7  | [Tab][Space][Space][Tab]   | -          | Subtraction                                                                             |
// 8  | [Tab][Space][Space][LF]    | -          | Multiplication                                                                          |
// 9  | [Tab][Space][Tab][Space]   | -          | Integer Division                                                                        |
// 10 | [Tab][Space][Tab][Tab]     | -          | Modulo                                                                                  |
// 11 | [Tab][Tab][Space]          | -          | Store                                                                                   |
// 12 | [Tab][Tab][Tab]            | -          | Retrieve                                                                                |
// 13 | [Tab][LF][Space][Space]    | -          | Output the character at the top of the stack                                            |
// 14 | [Tab][LF][Space][Tab]      | -          | Output the number at the top of the stack                                               |
// 15 | [Tab][LF][Tab][Space]      | -          | Read a character and place it in the location given by the top of the stack             |
// 16 | [Tab][LF][Tab][Tab]        | -          | Read a number and place it in the location given by the top of the stack                |
//    | [LF][Space][Space]         | Label      | Mark a location in the program                                                          |
// 17 | [LF][Space][Tab]           | Label      | Call a subroutine                                                                       |
// 18 | [LF][Space][LF]            | Label      | Jump unconditionally to a label                                                         |
// 19 | [LF][Tab][Space]           | Label      | Jump to a label if the top of the stack is zero                                         |
// 20 | [LF][Tab][Tab]             | Label      | Jump to a label if the top of the stack is negative                                     |
// 21 | [LF][Tab][LF]              | -          | End a subroutine and transfer control back to the caller                                |
// 22 | [LF][LF][LF]               | -          | End the program                                                                         |