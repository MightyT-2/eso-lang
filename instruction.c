#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "instruction.h"

#define CALL_STACK_SIZE          1024

instruction *program = 0;
instruction *curr_instruct = 0;

label *source_labels = 0;
label *curr_source_label = 0;

label *dest_labels = 0;
label *curr_dest_label = 0;

instruction *call_stack[CALL_STACK_SIZE] = {0};
int         call_stack_top = 0;

int add_instruction(int code, int param, char *p_label)
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
    curr_instruct->int_param = param;

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

    return 0;
}

int add_label(char *p_label)
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

    return 0;
}

int connect_labels()
{
    for (curr_source_label = source_labels; curr_source_label != 0; curr_source_label = curr_source_label->next_label)
    {
        curr_dest_label = dest_labels;
        while (curr_dest_label != 0 && strcmp(curr_dest_label->label_id, curr_source_label->label_id))
        {
            curr_dest_label = curr_dest_label->next_label;
        }
        while (curr_dest_label != 0 && curr_dest_label->instruct_loc == 0)
        {
            curr_dest_label = curr_dest_label->next_label;
        }
        if (curr_dest_label == 0)
        {
            return 1;
        }
        curr_source_label->instruct_loc->jump_instruct = curr_dest_label->instruct_loc;
    }

    return 0;
}

int start_program()
{
    curr_instruct = program;
}

int get_instruct(int64_t *param)
{
    if (curr_instruct == 0)
    {
        return -1;
    }
    *param = curr_instruct->int_param;
    return curr_instruct->instruct_num;
}

int next_instruct(int jump)
{
    if (jump)
    {
        curr_instruct = curr_instruct->jump_instruct;
    }
    else
    {
        curr_instruct = curr_instruct->next_instruct;
    }

    return 0;
}

int enter_subroutine()
{
    cs_push(curr_instruct->next_instruct);
    curr_instruct = curr_instruct->jump_instruct;

    return 0;
}

int return_subroutine()
{
    return cs_pop(&curr_instruct);
}

int cs_push(instruction *item)
{
    if (call_stack_top >= CALL_STACK_SIZE)
    {
        return 1;
    }
    call_stack[call_stack_top] = item;
    call_stack_top++;
    
    return 0;
}

/* TODO: error handling */
int cs_pop(instruction **item)
{
    if (call_stack_top <= 0)
    {
        return 1;
    }
    call_stack_top--;
    if (item != 0)
    {
        *item = call_stack[call_stack_top];
    }
    
    return 0;
}

/* TODO: error handling */
int cs_peek(int index, instruction **item)
{
    if (index > call_stack_top)
    {
        return 1;
    }
    *item = call_stack[call_stack_top - index];

    return 0;
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

