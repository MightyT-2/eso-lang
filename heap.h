#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#ifndef _ERROR
#include "error.h"
#endif

#define _HEAP

ERROR_CODE fetch(int index, int64_t *item);
void store(int index, int64_t item);

/* Debug function */
void heap_dump();
