// Whitespace Interpreter
// If no arguments are passed, it should act as a shell, similar to JShell or just typing Python in the terminal

// TODO: shell
// TODO: proper error handling
// TODO: memory management

#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

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
#define STACK_SIZE               1024
#define ERR_STACK_OVERFLOW       "Exiting. Stack overflow.\n"
#define ERR_STACK_UNDERFLOW      "Exiting. Stack underflow.\n"
#define ERR_CALL_STACK_OVERFLOW  "Exiting. Stack overflow.\n"
#define ERR_CALL_STACK_UNDERFLOW "Exiting. Stack underflow.\n"
#define ERR_INVAL_INSTRUCT       "Exiting. Invalid instruction.\n"
#define ERR_DIV_ZERO             "Exiting. Divide by zero.\n"
#define ERR_NOT_NUMBER           "Exiting. Not a number.\n"
#define HERE                     "here\n"
#define STD_INPUT                1

typedef struct termios termios;

typedef struct strchar
{
    char           character;
    struct strchar *next_character;
} strchar;

typedef struct instruction
{
    uint8_t            instruct_num;
    int64_t            int_param;
    char               *label_param;
    struct instruction *next_instruct;
} instruction;

typedef struct label
{
    instruction  *instruct_loc;
    char         *label_id;
    struct label *next_label,
                 *prev_label;
} label;

typedef struct heap_entry
{
    int64_t           heap_value;
    uint64_t          entry_id;
    struct heap_entry *next_entry,
                      *prev_entry;
} heap_entry;

typedef struct process
{
    instruction *first_instruct,
                *curr_instruct;
    label       *first_label,
                *last_label,
                *curr_label;
    int64_t     *stack_base,
                *stack_entry;
    heap_entry  *curr_heap_entry;
} process;

/* Runs the whitespace shell */
int ws_shell();

/* Searches a file pointer for the next whitespace character */
char find_next_token(FILE *source_file);

/* Converts whitespace bits to an instructor label */
char *get_label(FILE *source_file);

/* Converts whitespace bits to a 64 bit integer */
int64_t get_int(FILE *source_file);

/* Initialize the whitespace process */
process ws_init_proc();

/* Kills the whitespace process */
void ws_kill_proc(process kill_proc);

/* Debug function used to show the contents of the stack */
void debug_stack_dump(int64_t *start_stack, int64_t *end_stack);

/* Debug function used to show the contents of the heap */
void debug_heap_dump(heap_entry *cur_heap_entry);

/* Debug function used to step through each instruction */
void debug_step();

/* Loads the instructions into a process */
int ws_load(FILE *source_file, process *dispatch_proc);

/* Run a whitespace process */
int ws_run(process running_process);

/* Convert the whitespace source code to a legible format */
void ws_convert(char *file_name);

/* Echos the whitespace source code with character coloring */
int ws_echo(char* file_name);

// TODO: Configure each of the options to handle their own processes
int main(int argc, char *argv[])
{
    process user_process;
    FILE    *source_file;

    /* Shell */
    if (argc == 1)
    {
        ws_shell();
    }

    /* Interpreter */
    else if (strcmp(argv[1], "run") == 0)
    {
        source_file = fopen(argv[2], "r");
        user_process = ws_init_proc();
        ws_load(source_file, &user_process);
        ws_run(user_process);
        ws_kill_proc(user_process);
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

/* Debug function used to show the contents of the heap */
void debug_heap_dump(heap_entry *cur_heap_entry)
{
    printf("Heap Dump:\n");
    if (cur_heap_entry == NULL)
    {
        return;
    }
    while (cur_heap_entry->prev_entry != NULL)
    {
        cur_heap_entry = cur_heap_entry->prev_entry;
    }
    while (cur_heap_entry->next_entry != NULL)
    {
        printf("%05ld: %ld\n", cur_heap_entry->entry_id, cur_heap_entry->heap_value);
        cur_heap_entry = cur_heap_entry->next_entry;
    }
    printf("%05ld: %ld\n", cur_heap_entry->entry_id, cur_heap_entry->heap_value);
    return;
}

/* Debug function used to show the contents of the stack */
void debug_stack_dump(int64_t *start_stack, int64_t *end_stack)
{
    int64_t *cur_stack = start_stack;

    printf("Stack Dump:\n");
    while (cur_stack != end_stack)
    {
        printf("%05ld: %ld\n", cur_stack - start_stack, *cur_stack);
        cur_stack++;
    }
    return;
}

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
            printf("Exiting. Invalid integer.\n");
            exit(1);
        }
        if (digit_count > 64)
        {
            printf("Exiting. Integer too large.\n");
            exit(1);
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
            exit(1);
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

/* Initialize the whitespace process */
process ws_init_proc()
{
    process new_proc;

    /* Initialize the stack */
    if ((new_proc.stack_base = (int64_t *)calloc(STACK_SIZE, sizeof(int64_t))) == NULL)
    {
        printf("Exiting. Insufficient memory for stack allocation\n");
        return new_proc;
    }
    new_proc.stack_entry     = new_proc.stack_base;
    new_proc.first_instruct  = NULL;
    new_proc.curr_instruct   = NULL;
    new_proc.first_label     = NULL;
    new_proc.last_label      = NULL;
    new_proc.curr_label      = NULL;
    new_proc.curr_heap_entry = NULL;

    return new_proc;
}

/* Kills the whitespace process */
void ws_kill_proc(process kill_proc)
{
    
    /* Free the instructions */
    while (kill_proc.first_instruct != NULL)
    {
        kill_proc.curr_instruct = kill_proc.first_instruct->next_instruct;
        free(kill_proc.first_instruct);
        kill_proc.first_instruct = kill_proc.curr_instruct;
    }

    /* Free the instruction labels */
    while (kill_proc.first_label != NULL)
    {
        kill_proc.curr_label = kill_proc.first_label->next_label;
        free(kill_proc.first_label->label_id);
        free(kill_proc.first_label);
        kill_proc.first_label = kill_proc.curr_label;
    }
    kill_proc.last_label = NULL;

    /* Free the stack */
    free(kill_proc.stack_base);
    kill_proc.stack_base = NULL;
    kill_proc.stack_entry = NULL;

    /* Free the heap */
    if (kill_proc.curr_heap_entry != NULL)
    {
        while (kill_proc.curr_heap_entry->prev_entry != NULL)
        {
            kill_proc.curr_heap_entry = kill_proc.curr_heap_entry->prev_entry;
        }
        while (kill_proc.curr_heap_entry->next_entry != NULL)
        {
            kill_proc.curr_heap_entry = kill_proc.curr_heap_entry->next_entry;
            free(kill_proc.curr_heap_entry->prev_entry);
        }
        free(kill_proc.curr_heap_entry);
        kill_proc.curr_heap_entry = NULL;
    }

    return;
}

/* Loads the instructions into a process */
int ws_load(FILE *source_file, process *dispatch_proc)
{
    char        next_char;
    instruction *current_instruct = NULL,
                *previous_instruct = NULL;
    label       *curr_label = NULL;
    int         is_label = 0;
    
    printf("here\r\n");
    /* Loop until an end of file character is encountered */
    while ((next_char = find_next_token(source_file)) != EOF)
    {

        /* Create the instruction list */
        if (current_instruct == NULL && curr_label == NULL)
        {
            dispatch_proc->first_instruct = (instruction *)calloc(1, sizeof(instruction));
            dispatch_proc->first_label = (label *)calloc(1, sizeof(label));
            current_instruct = dispatch_proc->first_instruct;
            curr_label = dispatch_proc->first_label;
        }
        else
        {
            if(is_label != 1)
            {
                if(is_label == 2)
                {
                    curr_label->instruct_loc = current_instruct;
                    curr_label->next_label = (label *)calloc(1, sizeof(label));
                    curr_label->next_label->prev_label = curr_label;
                    curr_label = curr_label->next_label;
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
            }
            
            else if (next_char == '\t')
            {
                // 2  | [Space][Tab][Space] | Number | Copy the nth item on the stack (given by the argument) onto the top of the stack (v0.3) |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 2;
                    current_instruct->int_param = get_int(source_file);
                }
                
                // 3  | [Space][Tab][LF] | Number | Slide n items off the stack, keeping the top item (v0.3) |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 3;
                    current_instruct->int_param = get_int(source_file);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    return 1;
                }
            }
            
            else if (next_char == '\n')
            {
                // 4  | [Space][LF][Space] | - | Duplicate the top item on the stack |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 4;
                }
                
                // 5  | [Space][LF][Tab] | - | Swap the top two items on the stack |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 5;
                }
                
                // 6  | [Space][LF][LF] | - | Discard the top item on the stack |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 6;
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    return 1;
                }
            }
            
            else
            {
                printf(ERR_INVAL_INSTRUCT);
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
                        current_instruct->instruct_num = 7;
                    }
                    
                    // 8  | [Tab][Space][Space][Tab] | - | Subtraction |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 8;
                    }
                    
                    // 9  | [Tab][Space][Space][LF] | - | Multiplication |
                    else if (next_char == '\n')
                    {
                        current_instruct->instruct_num = 9;
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        return 1;
                    }
                }
                
                else if (next_char == '\t')
                {
                    // 10 | [Tab][Space][Tab][Space] | - | Integer Division |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        current_instruct->instruct_num = 10;
                    }
                    
                    // 11 | [Tab][Space][Tab][Tab] | - | Modulo |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 11;
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        return 1;
                    }
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    return 1;
                }
            }
            
            /* Heap Access */
            else if (next_char == '\t')
            {
                // 12 | [Tab][Tab][Space] | - | Store |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 12;
                }
                
                // 13 | [Tab][Tab][Tab] | - | Retrieve |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 13;
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
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
                        current_instruct->instruct_num = 14;
                    }
                    
                    // 15 | [Tab][LF][Space][Tab] | - | Output the number at the top of the stack |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 15;
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        return 1;
                    }
                }
                
                else if (next_char == '\t')
                {
                    // 16 | [Tab][LF][Tab][Space] | - | Read a character and place it in the location given by the top of the stack |
                    if ((next_char = find_next_token(source_file)) == ' ')
                    {
                        current_instruct->instruct_num = 16;
                    }
                    
                    // 17 | [Tab][LF][Tab][Tab] | - | Read a number and place it in the location given by the top of the stack |
                    else if (next_char == '\t')
                    {
                        current_instruct->instruct_num = 17;
                    }
                    
                    else
                    {
                        printf(ERR_INVAL_INSTRUCT);
                        return 1;
                    }
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    return 1;
                }
            }
            
            else
            {
                printf(ERR_INVAL_INSTRUCT);
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
                    curr_label->label_id = get_label(source_file);
                    is_label = 1;
                }
                
                // 18 | [LF][Space][Tab] | Label | Call a subroutine |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 18;
                    current_instruct->label_param = get_label(source_file);
                }
                
                // 19 | [LF][Space][LF] | Label | Jump unconditionally to a label |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 19;
                    current_instruct->label_param = get_label(source_file);
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    return 1;
                }
                
            }
            
            else if (next_char == '\t')
            {
                // 20 | [LF][Tab][Space] | Label | Jump to a label if the top of the stack is zero |
                if ((next_char = find_next_token(source_file)) == ' ')
                {
                    current_instruct->instruct_num = 20;
                    current_instruct->label_param = get_label(source_file);
                }
                
                // 21 | [LF][Tab][Tab] | Label | Jump to a label if the top of the stack is negative |
                else if (next_char == '\t')
                {
                    current_instruct->instruct_num = 21;
                    current_instruct->label_param = get_label(source_file);
                }
                
                // 22 | [LF][Tab][LF] | - | End a subroutine and transfer control back to the caller |
                else if (next_char == '\n')
                {
                    current_instruct->instruct_num = 22;
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    return 1;
                }
                
            }
            
            else if (next_char == '\n')
            {
                // 23 | [LF][LF][LF] | - | End the program |
                if ((next_char = find_next_token(source_file)) == '\n')
                {
                    current_instruct->instruct_num = 23;
                }
                
                else
                {
                    printf(ERR_INVAL_INSTRUCT);
                    return 1;
                }
            }
            
            else
            {
                printf(ERR_INVAL_INSTRUCT);
                return 1;
            }
        }
        
        else
        {
            printf(ERR_INVAL_INSTRUCT);
            return 1;
        }
    }
    if (previous_instruct == NULL)
    {
        printf("Exiting. No instructions have been generated.\n");
        return 1;
    }
    
    if (is_label == 2)
    {
        printf("Exiting. Label pointing past instructions.\n");
        return 1;
    }

    dispatch_proc->curr_instruct = dispatch_proc->first_instruct;

    return 0;
}

/* Run a whitespace process */
int ws_run(process running_process)
{
    instruction **call_stack_base,
                **call_stack_entry;
    int64_t     register_1,
                register_2,
                register_3;
    // int         debug_int = 0;

    /* Initialize the call stack */
    if ((call_stack_base = (instruction **)malloc(STACK_SIZE * sizeof(instruction **))) == NULL)
    {
        printf("Exiting. Not enough memory for call stack allocation\n");
        return 1;
    }
    call_stack_entry = call_stack_base;
    
    /* Loop until an end process instruction is reached */
    while (running_process.curr_instruct->instruct_num != 23)
    {

        /* Debug Messages */
        // printf("%d\n", debug_int++);
        // printf("here %d\n", running_process.curr_instruct->instruct_num);
        // printf("Call Stack Dump:\nLength: %ld\n", call_stack_entry - call_stack_base);
        // debug_heap_dump(running_process.curr_heap_entry);
        // debug_stack_dump(running_process.stack_base, running_process.stack_entry);
        // printf("Current instruction: %2d, Int param: %ld\n", running_process.curr_instruct->instruct_num, running_process.curr_instruct->int_param);
        // debug_step();

        /* Exit if an impossible instruction is encountered */
        if (running_process.curr_instruct->instruct_num > 23)
        {
            printf(ERR_INVAL_INSTRUCT);
            return 1;
        }


        /* Push a number to the stack */
        if (running_process.curr_instruct->instruct_num == 1)
        {
            if (running_process.stack_entry - running_process.stack_base > STACK_SIZE)
            {
                printf(ERR_STACK_OVERFLOW);
                return 1;
            }
            *running_process.stack_entry = running_process.curr_instruct->int_param;
            running_process.stack_entry++;
        }

        /* Copy the nth item on the stack */
        else if (running_process.curr_instruct->instruct_num == 2)
        {
            if (running_process.stack_entry < running_process.stack_base + running_process.curr_instruct->int_param)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            *running_process.stack_entry = *(running_process.stack_entry - running_process.curr_instruct->int_param - 1);
            running_process.stack_entry++;
        }

        /* Slide n items of the stack while keeping the top item */
        else if (running_process.curr_instruct->instruct_num == 3)
        {
            if (running_process.stack_entry < running_process.stack_base + running_process.curr_instruct->int_param)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            for (register_1 = running_process.curr_instruct->int_param; register_1 > 0; register_1--)
            {
                running_process.stack_entry--;
                *(running_process.stack_entry - 1) = *running_process.stack_entry;
                *running_process.stack_entry = 0;
            }
        }

        /* Duplicate the top stack item */
        else if (running_process.curr_instruct->instruct_num == 4)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (running_process.stack_entry - running_process.stack_base > STACK_SIZE)
            {
                printf(ERR_STACK_OVERFLOW);
                return 1;
            }
            *running_process.stack_entry = *(running_process.stack_entry - 1);
            running_process.stack_entry++;
        }

        /* Swap the top two items on the stack */
        else if (running_process.curr_instruct->instruct_num == 5)
        {
            if (running_process.stack_entry < running_process.stack_base + 2)
            {
                printf(ERR_STACK_UNDERFLOW);
            }
            register_1 = *(running_process.stack_entry - 1);
            *(running_process.stack_entry - 1) = *(running_process.stack_entry - 2);
            *(running_process.stack_entry - 2) = register_1;
        }

        /* Discard the top item off the stack */
        else if (running_process.curr_instruct->instruct_num == 6)
        {
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }

        /* Perform addition with the top two items on the stack */
        else if (running_process.curr_instruct->instruct_num == 7)
        {
            if (running_process.stack_entry < running_process.stack_base + 2)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            *(running_process.stack_entry - 2) += *(running_process.stack_entry - 1);
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }

        /* Perform subtraction with the top two items on the stack */
        else if (running_process.curr_instruct->instruct_num == 8)
        {
            if (running_process.stack_entry < running_process.stack_base + 2)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            *(running_process.stack_entry - 2) -= *(running_process.stack_entry - 1);
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }

        /* Perform multiplication with the top two items on the stack */
        else if (running_process.curr_instruct->instruct_num == 9)
        {
            if (running_process.stack_entry < running_process.stack_base + 2)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            *(running_process.stack_entry - 2) *= *(running_process.stack_entry - 1);
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }

        /* Perform integer division with the top two items on the stack */
        else if (running_process.curr_instruct->instruct_num == 10)
        {
            if (running_process.stack_entry < running_process.stack_base + 2)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (*(running_process.stack_entry - 1) == 0)
            {
                printf(ERR_DIV_ZERO);
                return 1;
            }
            *(running_process.stack_entry - 2) /= *(running_process.stack_entry - 1);
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }

        /* Perform modulo with the top two items on the stack */
        else if (running_process.curr_instruct->instruct_num == 11)
        {
            if (running_process.stack_entry < running_process.stack_base + 2)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (*(running_process.stack_entry - 1) == 0)
            {
                printf(ERR_DIV_ZERO);
                return 1;
            }
            *(running_process.stack_entry - 2) %= *(running_process.stack_entry - 1);
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }

        /* Store the top item on the stack in a heap entry */
        else if (running_process.curr_instruct->instruct_num == 12)
        {
            if (running_process.stack_entry < running_process.stack_base + 2)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (running_process.curr_heap_entry == NULL)
            {
                running_process.curr_heap_entry = calloc(1, sizeof(heap_entry));
                running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 2);
            }
            else if (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 2))
            {
                while (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 2) && running_process.curr_heap_entry->prev_entry != NULL)
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                }
                if (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 2))
                {
                    running_process.curr_heap_entry->next_entry->prev_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->next_entry->prev_entry->prev_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry->next_entry->prev_entry->next_entry = running_process.curr_heap_entry->next_entry;
                    running_process.curr_heap_entry->next_entry = running_process.curr_heap_entry->next_entry->prev_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 2);
                }
                else if (running_process.curr_heap_entry->prev_entry == NULL && running_process.curr_heap_entry->entry_id != (uint64_t) *(running_process.stack_entry - 2))
                {
                    running_process.curr_heap_entry->prev_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->prev_entry->next_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 2);
                }
            }
            else if (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 2))
            {
                while (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 2) && running_process.curr_heap_entry->next_entry != NULL)
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                }
                if (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 2))
                {
                    running_process.curr_heap_entry->prev_entry->next_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->prev_entry->next_entry->next_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry->prev_entry->next_entry->prev_entry = running_process.curr_heap_entry->prev_entry;
                    running_process.curr_heap_entry->prev_entry = running_process.curr_heap_entry->prev_entry->next_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 2);
                }
                else if (running_process.curr_heap_entry->next_entry == NULL && running_process.curr_heap_entry->entry_id != (uint64_t) *(running_process.stack_entry - 2))
                {
                    running_process.curr_heap_entry->next_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->next_entry->prev_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 2);
                }
            }
            running_process.curr_heap_entry->heap_value = *(running_process.stack_entry - 1);
            running_process.stack_entry -= 2;
            *running_process.stack_entry = 0;
            *(running_process.stack_entry + 1) = 0;
        }

        /* Retrieve an item from a heap entry */
        else if (running_process.curr_instruct->instruct_num == 13)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (running_process.curr_heap_entry == NULL)
            {
                printf("Exiting. No heap entrys.\n");
                return 1;
            }
            else if (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1))
            {
                while (running_process.curr_heap_entry->prev_entry != NULL && running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                }
            }
            else if (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1))
            {
                while (running_process.curr_heap_entry->next_entry != NULL && running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                }
            }
            if (running_process.curr_heap_entry->entry_id != (uint64_t) *(running_process.stack_entry - 1))
            {
                printf("Exiting. No heap entry %ld.\n", *(running_process.stack_entry - 1));
                debug_stack_dump(running_process.stack_base, running_process.stack_entry);
                debug_heap_dump(running_process.curr_heap_entry);
                return 1;
            }
            *(running_process.stack_entry - 1) = running_process.curr_heap_entry->heap_value;

        }
        
        /* Output the character at the top of the stack */
        else if (running_process.curr_instruct->instruct_num == 14)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            printf("%lc", (int) *(running_process.stack_entry - 1));
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }
        
        /* Output the integer at the top of the stack */
        else if (running_process.curr_instruct->instruct_num == 15)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            printf("%d", (int) *(running_process.stack_entry - 1));
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }
        
        /* Read a character and place it in a heap entry */
        else if (running_process.curr_instruct->instruct_num == 16)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (running_process.curr_heap_entry == NULL)
            {
                running_process.curr_heap_entry = calloc(1, sizeof(heap_entry));
                running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 1);
            }
            else if (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1))
            {
                while (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1) && running_process.curr_heap_entry->prev_entry != NULL)
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                }
                if (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->next_entry->prev_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->next_entry->prev_entry->prev_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry->next_entry->prev_entry->next_entry = running_process.curr_heap_entry->next_entry;
                    running_process.curr_heap_entry->next_entry = running_process.curr_heap_entry->next_entry->prev_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                }
                else if (running_process.curr_heap_entry->prev_entry == NULL && running_process.curr_heap_entry->entry_id != (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->prev_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->prev_entry->next_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                }
            }
            else if (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1))
            {
                while (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1) && running_process.curr_heap_entry->next_entry != NULL)
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                }
                if (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->prev_entry->next_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->prev_entry->next_entry->next_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry->prev_entry->next_entry->prev_entry = running_process.curr_heap_entry->prev_entry;
                    running_process.curr_heap_entry->prev_entry = running_process.curr_heap_entry->prev_entry->next_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                }
                else if (running_process.curr_heap_entry->next_entry == NULL && running_process.curr_heap_entry->entry_id != (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->next_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->next_entry->prev_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                }
            }
            running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 1);
            scanf("%1lc", (int *)&(running_process.curr_heap_entry->heap_value));
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }
        
        /* Read an integer and place it in a heap entry */
        else if (running_process.curr_instruct->instruct_num == 17)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (running_process.curr_heap_entry == NULL)
            {
                running_process.curr_heap_entry = calloc(1, sizeof(heap_entry));
            }
            else if (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1))
            {
                while (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1) && running_process.curr_heap_entry->prev_entry != NULL)
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                }
                if (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->next_entry->prev_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->next_entry->prev_entry->prev_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry->next_entry->prev_entry->next_entry = running_process.curr_heap_entry->next_entry;
                    running_process.curr_heap_entry->next_entry = running_process.curr_heap_entry->next_entry->prev_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 1);
                }
                else if (running_process.curr_heap_entry->prev_entry == NULL && running_process.curr_heap_entry->entry_id != (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->prev_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->prev_entry->next_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 1);
                }
            }
            else if (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1))
            {
                while (running_process.curr_heap_entry->entry_id < (uint64_t) *(running_process.stack_entry - 1) && running_process.curr_heap_entry->next_entry != NULL)
                {
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                }
                if (running_process.curr_heap_entry->entry_id > (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->prev_entry->next_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->prev_entry->next_entry->next_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry->prev_entry->next_entry->prev_entry = running_process.curr_heap_entry->prev_entry;
                    running_process.curr_heap_entry->prev_entry = running_process.curr_heap_entry->prev_entry->next_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->prev_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 1);
                }
                else if (running_process.curr_heap_entry->next_entry == NULL && running_process.curr_heap_entry->entry_id != (uint64_t) *(running_process.stack_entry - 1))
                {
                    running_process.curr_heap_entry->next_entry = calloc(1, sizeof(heap_entry));
                    running_process.curr_heap_entry->next_entry->prev_entry = running_process.curr_heap_entry;
                    running_process.curr_heap_entry = running_process.curr_heap_entry->next_entry;
                    running_process.curr_heap_entry->entry_id = *(running_process.stack_entry - 1);
                }
            }
            register_1 = 0;
            register_2 = 0;
            scanf("%lc", (int *)&register_2);
            if (register_2 == '-')
            {
                register_3 = 1;
                scanf("%lc", (int *)&register_2);
            }
            else
            {
                register_3 = 0;
            }
            while (register_2 != '\n')
            {
                if (register_2 < '0' || register_2 > '9')
                {
                    printf(ERR_NOT_NUMBER);
                    return 1;
                }
                register_1 *= 10;
                register_1 += register_2 - '0';
                scanf("%lc", (int *)&register_2);
            }
            if (register_3)
            {
                register_1 = -register_1;
            }
            running_process.curr_heap_entry->heap_value = register_1;
            running_process.stack_entry--;
            *running_process.stack_entry = 0;
        }
        
        /* Call a subroutine */
        if (running_process.curr_instruct->instruct_num == 18)
        {
            if (running_process.first_label == NULL)
            {
                printf("Exiting. There are no labels.\n");
                return 1;
            }
            running_process.curr_label = running_process.first_label;
            while (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0 && running_process.curr_label != running_process.last_label)
            {
                running_process.curr_label = running_process.curr_label->next_label;
            }
            if (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0)
            {
                printf("Exiting. No label matching requested label.\n");
                return 1;
            }
            if (call_stack_entry - call_stack_base > STACK_SIZE)
            {
                printf(ERR_STACK_OVERFLOW);
                return 1;
            }
            *call_stack_entry = running_process.curr_instruct;
            call_stack_entry++;
            running_process.curr_instruct = running_process.curr_label->instruct_loc;
        }
        
        /* Jump to a label unconditionally */
        else if (running_process.curr_instruct->instruct_num == 19)
        {
            if (running_process.first_label == NULL)
            {
                printf("Exiting. There are no labels.\n");
                return 1;
            }
            running_process.curr_label = running_process.first_label;
            while (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0 && running_process.curr_label != running_process.last_label)
            {
                running_process.curr_label = running_process.curr_label->next_label;
            }
            if (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0)
            {
                printf("Exiting. No label matching requested label.\n");
                return 1;
            }
            if (call_stack_entry - call_stack_base > STACK_SIZE)
            {
                printf(ERR_STACK_OVERFLOW);
                return 1;
            }
            running_process.curr_instruct = running_process.curr_label->instruct_loc;
        }
        
        /* Jump to a label if the top of the stack is zero */
        else if (running_process.curr_instruct->instruct_num == 20)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (running_process.first_label == NULL)
            {
                printf("Exiting. There are no labels.\n");
                return 1;
            }
            running_process.curr_label = running_process.first_label;
            while (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0 && running_process.curr_label != running_process.last_label)
            {
                running_process.curr_label = running_process.curr_label->next_label;
            }
            if (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0)
            {
                printf("Exiting. No label matching requested label.\n");
                return 1;
            }
            if (call_stack_entry - call_stack_base > STACK_SIZE)
            {
                printf(ERR_STACK_OVERFLOW);
                return 1;
            }
            if (*(running_process.stack_entry - 1) == 0)
            {
                running_process.curr_instruct = running_process.curr_label->instruct_loc;
            }
            else
            {
                if (running_process.curr_instruct->next_instruct == NULL)
                {
                    printf("Exiting. Terminating program with no end instruction.\n");
                    return 1;
                }
                running_process.curr_instruct = running_process.curr_instruct->next_instruct;
            }
            running_process.stack_entry--;
        }
        
        /* Jump to a label if the top of the stack is negative */
        else if (running_process.curr_instruct->instruct_num == 21)
        {
            if (running_process.stack_entry < running_process.stack_base + 1)
            {
                printf(ERR_STACK_UNDERFLOW);
                return 1;
            }
            if (running_process.first_label == NULL)
            {
                printf("Exiting. There are no labels.\n");
                return 1;
            }
            running_process.curr_label = running_process.first_label;
            while (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0 && running_process.curr_label != running_process.last_label)
            {
                running_process.curr_label = running_process.curr_label->next_label;
            }
            if (strcmp(running_process.curr_label->label_id, running_process.curr_instruct->label_param) != 0)
            {
                printf("Exiting. No label matching requested label.\n");
                return 1;
            }
            if (call_stack_entry - call_stack_base > STACK_SIZE)
            {
                printf(ERR_STACK_OVERFLOW);
                return 1;
            }
            if (*(running_process.stack_entry - 1) < 0)
            {
                running_process.curr_instruct = running_process.curr_label->instruct_loc;
            }
            else
            {
                if (running_process.curr_instruct->next_instruct == NULL)
                {
                    printf("Exiting. Terminating program with no end instruction.\n");
                    return 1;
                }
                running_process.curr_instruct = running_process.curr_instruct->next_instruct;
            }
            running_process.stack_entry--;
        }
        
        /* Return from a subroutine */
        else if (running_process.curr_instruct->instruct_num == 22)
        {
            if (call_stack_entry < call_stack_base + 1)
            {
                printf(ERR_CALL_STACK_UNDERFLOW);
                return 1;
            }
            call_stack_entry--;
            running_process.curr_instruct = *call_stack_entry;
            *call_stack_entry = 0;
            if (running_process.curr_instruct->next_instruct == NULL)
            {
                printf("Exiting. Terminating process with no end instruction.\n");
                return 1;
            }
            running_process.curr_instruct = running_process.curr_instruct->next_instruct;
        }
        
        /* End running if there is no end program instruction */
        else
        {
            if (running_process.curr_instruct->next_instruct == NULL)
            {
                printf("Exiting. Terminating program with no end instruction.\n");
                return 1;
            }
            running_process.curr_instruct = running_process.curr_instruct->next_instruct;
        }
    }

    return 0;
}

/* Runs the whitespace shell */
/* In progress */
int ws_shell()
{
    process shell_proc;
    int     pipeline[2];
    FILE    *write_buffer;
    FILE    *read_buffer;
    char    next_char;
    termios new_termios;
    termios old_termios;
    
    tcgetattr(STD_INPUT, &old_termios);
    cfmakeraw(&new_termios);
    // new_termios.c_iflag      = 0;
    // new_termios.c_oflag      = 0;
    // new_termios.c_cflag      = 0;
    // new_termios.c_lflag      = 0;
    // new_termios.c_cc[VEOF]   = 4adsf;
    // new_termios.c_cc[VEOL]   = 0;
    // new_termios.c_cc[VERASE] = 0;
    // new_termios.c_cc[VINTR]  = 0;
    // new_termios.c_cc[VKILL]  = 0;
    // new_termios.c_cc[VMIN]   = 0;
    // new_termios.c_cc[VQUIT]  = 0;
    // new_termios.c_cc[VSTART] = 0;
    // new_termios.c_cc[VSTOP]  = 0;
    // new_termios.c_cc[VSUSP]  = 0;
    // new_termios.c_cc[VTIME]  = 0;
    tcsetattr(STD_INPUT, 0, &new_termios);

    // Begin processing if EOF is received.
    // Exit if EOF is received with no other input
    printf("A Whitespace Shell\n");
    printf("Enter EOF to execute code the entered code.\n");
    printf("Enter EOF with no other input to exit the shell.\n");
    shell_proc = ws_init_proc();

    pipe(pipeline);
    write_buffer = fdopen(pipeline[1], "w");
    read_buffer  = fdopen(pipeline[0], "r");

    while (1)
    {
        do
        {
            read(STD_INPUT, &next_char, sizeof(char));
            fputc(next_char, write_buffer);
            if (next_char == 27)
            {
                read(STD_INPUT, &next_char, sizeof(char));
                fputc(next_char, write_buffer);
                if (next_char == 91)
                {
                    read(STD_INPUT, &next_char, sizeof(char));
                    fputc(next_char, write_buffer);
                    if (next_char == 65)
                    {
                        printf("\033[1A");
                    }
                    else if (next_char == 66)
                    {
                        printf("\033[1B");
                        // B
                    }
                    else if (next_char == 67)
                    {
                        printf("\033[1C");
                        // C
                    }
                    else if (next_char == 68)
                    {
                        printf("\033[1D");
                        // D
                    }
                }
            }
            else if (next_char == 127)
            {
                printf("\033[1A");
            }
            else
            {
                if (next_char == ' ' || next_char == '\n' || next_char == '\t')
                {
                    fputc(next_char, write_buffer);
                }
                printf( "here %d\r\n", next_char);
            }
        }
        while(next_char != 4);
        // fputc(3, write_buffer);
        close(write_buffer);
        if (ws_load(read_buffer, &shell_proc) == 0)
        {
            ws_run(shell_proc);
        }
        write_buffer = fdopen(pipeline[1], "w");
    }
    ws_kill_proc(shell_proc);

    tcsetattr(STD_INPUT, 0, &old_termios);

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