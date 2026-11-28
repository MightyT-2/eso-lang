#include <stdint.h>
#ifndef _ERROR
#include "error.h"
#endif

#define _INSTRUCTION

void add_instruction(int code, int64_t param, char *p_label, int line, int column);

void add_label(char *p_label);

ERROR_CODE connect_labels();

ERROR_CODE start_program();

void get_instruct(int *instruct_num, int64_t *param, int *line, int *column);

ERROR_CODE next_instruct(int jump);

ERROR_CODE enter_subroutine();

ERROR_CODE return_subroutine();
