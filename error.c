#include <stdio.h>
#ifndef _ERROR
#include "error.h"
#endif

typedef struct error
{
    ERROR_CODE code;
    int line;
    int column;
} ERROR;

ERROR error_list[100];
int curr_error_count = 0;

void log_error(ERROR_CODE code, int line, int column)
{
    if (code > no_error)
    {
        error_list[curr_error_count].code = code;
        error_list[curr_error_count].line = line;
        error_list[curr_error_count].column = column;
        curr_error_count++;
    }

    return;
}

int count_error()
{
    return curr_error_count;
}

void print_errors()
{
    int curr_error = 0;

    for (curr_error == 0; curr_error < curr_error_count; curr_error++)
    {
        printf("%s:%d:%d: ", "hanoi.ws", error_list[curr_error].line, error_list[curr_error].column);
        switch(error_list[curr_error].code)
        {
            case stack_underflow:
                printf("Stack underflow.\n");
                break;
            case stack_overflow:
                printf("Stack overflow.\n");
                break;
            case call_stack_overflow:
                printf("Call stack overflow.\n");
                break;
            case call_stack_underflow:
                printf("Call stack underflow.\n");
                break;
            case null_heap_entry:
                printf("Null heap entry requested.\n");
                break;
            case non_existent_label:
                printf("Non-existent label encountered\n");
                break;
            case no_instructions:
                printf("Program did not generate any instructions.\n");
                break;
            case no_more_instructions:
                printf("Program ran out of instructions.\n");
                break;
            case unexpected_token:
                printf("Unexpected token encountered.\n");
                break;
            default:
                printf("Unknown error.\n");
        }
        printf("\n");
    }
    return;
}
