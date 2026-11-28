#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#ifndef _INSTRUCTION
#include "instruction.h"
#endif
#ifndef _ERROR
#include "error.h"
#endif

#define CALL_STACK_SIZE 1024

typedef struct instruction
{
    int                instruct_num,
                       line,
                       column;
    int64_t            param;
    struct instruction *next_instruct;
    struct instruction *jump_instruct;
} instruction;

typedef struct label
{
    instruction  *instruct_loc;
    char         *label_id;
    struct label *next_label;
} label;

instruction *program = 0;
instruction *curr_instruct = 0;

label *source_labels = 0;
label *curr_source_label = 0;

label *dest_labels = 0;
label *curr_dest_label = 0;

instruction *call_stack[CALL_STACK_SIZE] = {0};
int         call_stack_top = 0;

ERROR_CODE cs_push(instruction *item)
{
    if (call_stack_top >= CALL_STACK_SIZE)
    {
        return call_stack_overflow;
    }
    call_stack[call_stack_top] = item;
    call_stack_top++;
    
    return no_error;
}

/* TODO: error handling */
ERROR_CODE cs_pop(instruction **item)
{
    if (call_stack_top <= 0)
    {
        return call_stack_underflow;
    }
    call_stack_top--;
    if (item != 0)
    {
        *item = call_stack[call_stack_top];
    }
    
    return no_error;
}

/* TODO: error handling */
ERROR_CODE cs_peek(int index, instruction **item)
{
    if (index > call_stack_top)
    {
        return call_stack_underflow;
    }
    *item = call_stack[call_stack_top - index];

    return no_error;
}

int cs_is_empty()
{
    if (call_stack_top == 0)
    {
        return 1;
    }

    return 0;
}

int cs_stack_size()
{
    return CALL_STACK_SIZE;
}

void add_instruction(int code, int64_t param, char *p_label, int line, int column)
{

    /* Add new instruction */
    if (program == 0)
    {
        program = calloc(1, sizeof(instruction));
        curr_instruct = program;
    }
    else
    {
        curr_instruct->next_instruct = calloc(1, sizeof(instruction));
        curr_instruct = curr_instruct->next_instruct;
    }
    curr_instruct->instruct_num = code;
    curr_instruct->param = param;
    curr_instruct->line = line;
    curr_instruct->column = column;

    /* Add label */
    if (p_label != 0)
    {
        if (source_labels == 0)
        {
            source_labels = calloc(1, sizeof(label));
            curr_source_label = source_labels;
        }
        else
        {
            curr_source_label->next_label = calloc(1, sizeof(label));
            curr_source_label = curr_source_label->next_label;
        }
        curr_source_label->label_id = p_label;
        curr_source_label->instruct_loc = curr_instruct;
    }

    /* Set label */
    if (curr_dest_label != 0)
    {
        if (curr_dest_label->instruct_loc == 0)
        {
            curr_dest_label->instruct_loc = curr_instruct;
        }
    }

    return;
}

void add_label(char *p_label)
{
    if (dest_labels == 0)
    {
        dest_labels = calloc(1, sizeof(label));
        curr_dest_label = dest_labels;
    }
    else
    {
        curr_dest_label->next_label = calloc(1, sizeof(label));
        curr_dest_label = curr_dest_label->next_label;
    }
    curr_dest_label->label_id = p_label;

    return;
}

ERROR_CODE connect_labels()
{
    for (curr_source_label = source_labels; curr_source_label != 0; curr_source_label = curr_source_label->next_label)
    {

        /* Cycle through destination labels */
        curr_dest_label = dest_labels;
        while (curr_dest_label != 0 && strcmp(curr_dest_label->label_id, curr_source_label->label_id))
        {
            curr_dest_label = curr_dest_label->next_label;
        }

        /* Skip layered labels */
        while (curr_dest_label != 0 && curr_dest_label->instruct_loc == 0)
        {
            curr_dest_label = curr_dest_label->next_label;
        }

        /* Label doesn't point to any instructions */
        if (curr_dest_label == 0)
        {
            return non_existent_label;
        }
        curr_source_label->instruct_loc->jump_instruct = curr_dest_label->instruct_loc;
    }

    return no_error;
}

ERROR_CODE start_program()
{
    curr_instruct = program;
    if (curr_instruct == NULL)
    {
        return no_instructions;
    }

    return no_error;
}

void get_instruct(int *instruct_num, int64_t *param, int *line, int *column)
{
    *param = curr_instruct->param;
    *instruct_num = curr_instruct->instruct_num;
    *line = curr_instruct->line;
    *column = curr_instruct->column;

    return;
}

ERROR_CODE next_instruct(int jump)
{
    if (jump)
    {
        curr_instruct = curr_instruct->jump_instruct;
    }
    else
    {
        curr_instruct = curr_instruct->next_instruct;
    }
    if (curr_instruct == 0)
    {
        return no_more_instructions;
    }

    return no_error;
}

ERROR_CODE enter_subroutine()
{
    ERROR_CODE error = cs_push(curr_instruct->next_instruct);

    curr_instruct = curr_instruct->jump_instruct;

    return error;
}

ERROR_CODE return_subroutine()
{
    return cs_pop(&curr_instruct);
}

