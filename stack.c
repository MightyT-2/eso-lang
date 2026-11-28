#include <stdint.h>
#include <stdio.h>
#ifndef _STACK
#include "stack.h"
#endif
#ifndef _ERROR
#include "error.h"
#endif

#define STACK_SIZE 1024

int64_t stack[STACK_SIZE] = {0};
int stack_top = 0;
int debug_pointer;

/* TODO: error handling */
ERROR_CODE push(int item)
{
    if (stack_top >= STACK_SIZE)
    {
        return stack_overflow;
    }
    stack[stack_top] = item;
    stack_top++;
    
    return no_error;
}

ERROR_CODE pop(int64_t *item)
{
    if (stack_top <= 0)
    {
        return stack_underflow;
    }
    stack_top--;
    if (item != 0)
    {
        *item = stack[stack_top];
    }
    
    return 0;
}

ERROR_CODE peek(int index, int64_t *item)
{
    if (index > stack_top)
    {
        return stack_underflow;
    }
    *item = stack[stack_top - (index + 1)];

    return no_error;
}

int is_empty()
{
    if (stack_top == 0)
    {
        return 1;
    }

    return 0;
}

int stack_size()
{
    return STACK_SIZE;
}

void stack_dump()
{
    printf("Stack Dump:\n");
    for (debug_pointer = 0; debug_pointer < stack_top; debug_pointer++)
    {
        printf("%4d %ld\n", debug_pointer, stack[debug_pointer]);
    }

    return;
}
