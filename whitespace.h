#include <stdio.h>
#include <stdint.h>
#ifndef _ERROR
#include "error.h"
#endif

#define _WHITESPACE

typedef enum instruction_codes
{
    instruct_push,
    instruct_copy,
    instruct_slide,
    instruct_dup,
    instruct_swap,
    instruct_discard,
    instruct_add,
    instruct_sub,
    instruct_multi,
    instruct_div,
    instruct_mod,
    instruct_store,
    instruct_fetch,
    instruct_output_c,
    instruct_output_i,
    instruct_input_c,
    instruct_input_i,
    instruct_call,
    instruct_jump,
    instruct_jump_zero,
    instruct_jump_neg,
    instruct_return,
    instruct_end
} INSTRUCTION_CODES;

/* Searches a file pointer for the next whitespace character */
char find_next_token(FILE *source_file);

/* Converts whitespace bits to an instructor label */
char *get_label(FILE *source_file);

/* Converts whitespace bits to a 64 bit integer */
int64_t get_int(FILE *source_file);

/* Debug function used to step through each instruction */
void debug_step();

/* Loads the instructions into a process */
int ws_load(FILE *source_file);

/* Run a whitespace process */
int ws_run();

/* Convert the whitespace source code to a legible format */
void ws_convert(char *file_name);

/* Echos the whitespace source code with character coloring */
int ws_echo(char* file_name);

/* Print user options */
void print_help();
