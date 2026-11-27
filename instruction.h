#include <stdint.h>

typedef struct instruction
{
    int                instruct_num;
    int64_t            int_param;
    struct instruction *next_instruct;
    struct instruction *jump_instruct;
} instruction;

typedef struct label
{
    instruction  *instruct_loc;
    char         *label_id;
    struct label *next_label,
                 *prev_label;
} label;

int add_instruction(int code, int param, char *p_label);

int add_label(char *p_label);

int connect_labels();

int start_program();

int get_instruct(int64_t *param);

int next_instruct(int jump);

int enter_subroutine();

int return_subroutine();

int cs_push(instruction *item);

/* TODO: error handling */
int cs_pop(instruction **item);

/* TODO: error handling */
int cs_peek(int index, instruction **item);

int cs_is_empty();

int cs_stack_size();
