#define _ERROR

typedef enum error_code
{
    no_error,
    stack_underflow,
    stack_overflow,
    call_stack_overflow,
    call_stack_underflow,
    null_heap_entry,
    non_existent_label,
    no_instructions,
    no_more_instructions,
    unexpected_token
} ERROR_CODE;

void log_error(ERROR_CODE code, int line, int column);

int count_error();

void print_errors();
