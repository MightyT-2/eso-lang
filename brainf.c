#include <stdio.h>

typedef struct cell
{
	char value;
	struct cell *p_next_cell;
	struct cell *p_prev_cell;
} cell;

typedef struct instruct
{
	char instruct;
	struct instruct *next_instruct;
	struct instruct *matching_instruct;
} instruct;

char get_next_token(FILE *source_file);

instruct bf_parse(instruct *p_program, FILE *source_file);

void bf_run();

int main()
{
	return 0;
}

char get_next_token(FILE *source_file)
{
	char next_char;

	while ((next_char = fgetc(source_file)) != EOF && next_char != '+' && next_char != '-' && next_char != '[' && next_char != ']' && next_char != '<' && next_char != '>' && next_char != ',' && next_char != '.');

	return next_char;
}

instruct bf_parse(instruct *p_program, FILE *source_file)
{
	instruct **loop_stack;
	instruct **curr_loop;
	instruct *curr_instruct;
	instruct *prev_instruct;
	char next_char;

	while ((next_char = get_next_token(source_file)) != EOF)
	{
		curr_instruct = calloc(sizeof(instruct), 1);
		curr_instruct->next_instruct = prev_instruct->next_instruct;
		prev_instruct->next_instruct = curr_instruct;
		if (next_char == '[')
		{

		}
		else if (next_char == ']')
		{

		}
		else
		{

		}
		prev_instruct = curr_instruct;
	}

}