#ifndef _ERROR
#include "error.h"
#endif

#define _STACK

ERROR_CODE push(int new_item);
ERROR_CODE pop(int64_t *item);
ERROR_CODE peek(int index, int64_t *item);
int is_empty();
int stack_size();

void stack_dump();
