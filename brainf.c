#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
typedef struct cell
{
	char value;
	struct cell *p_next_cell;
	struct cell *p_prev_cell;
} cell;

typedef struct instruct
{
	char instruct;
	struct instruct *p_next_instruct;
	struct instruct *p_matching_instruct;
} instruct;

typedef struct termios termios;

char get_next_token(FILE *source_file);

void bf_parse(instruct *p_process, FILE *source_file);

void bf_run(instruct *p_process);

cell *initialize_ribbon();

instruct *init_user_process();

int main(int argc, char *argv[])
{
	instruct *p_user_process;
	FILE *p_source_file;

	if (argc == 3)
	{
		if (strcmp(argv[1], "run") == 0)
		{
			p_source_file = fopen(argv[2], "r");
			p_user_process = init_user_process();
		}
	}

	return 0;
}

instruct *init_user_process()
{
	instruct *p_new_process;

	p_new_process = calloc(sizeof(instruct), 1);
	p_new_process->p_next_instruct = calloc(sizeof(instruct), 1);

	return p_new_process;
}

char get_next_token(FILE *source_file)
{
	char next_char;

	while ((next_char = fgetc(source_file)) != EOF && next_char != '+' && next_char != '-' && next_char != '[' && next_char != ']' && next_char != '<' && next_char != '>' && next_char != ',' && next_char != '.');

	return next_char;
}

void bf_parse(instruct *p_process, FILE *source_file)
{
	instruct **loop_stack = NULL;
	int loop_stack_count = 0;
	instruct *curr_instruct = NULL;
	instruct *prev_instruct = p_process;
	char next_char;

	while ((next_char = get_next_token(source_file)) != EOF)
	{
		curr_instruct = calloc(sizeof(instruct), 1);
		curr_instruct->instruct = next_char;
		curr_instruct->p_next_instruct = prev_instruct->p_next_instruct;
		prev_instruct->p_next_instruct = curr_instruct;
		if (next_char == '[')
		{
			loop_stack = realloc(loop_stack, (++loop_stack_count) * sizeof(instruct *));
			loop_stack[loop_stack_count - 1] = curr_instruct;
		}
		else if (next_char == ']')
		{
			if (loop_stack_count <= 0)
			{
				exit(1);
			}
			curr_instruct->p_matching_instruct = loop_stack[loop_stack_count];
			curr_instruct->p_matching_instruct->p_matching_instruct = curr_instruct;
			loop_stack = realloc(loop_stack, (--loop_stack_count) * sizeof(instruct *));
		}
		prev_instruct = curr_instruct;
	}

	if (loop_stack_count > 0)
	{
		exit(1);
	}

	return;
}

void bf_run(instruct *p_process)
{
	cell *p_curr_cell = initialize_ribbon();
	instruct *p_curr_instruct = p_process->p_next_instruct;

	while (p_curr_instruct->instruct != 0)
	{
		if (p_curr_instruct->instruct == '+')
		{
			p_curr_cell->value += 1;
		}
		else if (p_curr_instruct->instruct == '-')
		{
			p_curr_cell->value -= 1;
		}
		else if (p_curr_instruct->instruct == '>')
		{
			p_curr_cell = p_curr_cell->p_next_cell;
		}
		else if (p_curr_instruct->instruct == '<')
		{
			p_curr_cell = p_curr_cell->p_prev_cell;
		}
		else if (p_curr_instruct->instruct == '.')
		{
			printf("%c", p_curr_cell->value);
		}
		else if (p_curr_instruct->instruct == ',')
		{
			scanf("%1c");
		}

		if ((p_curr_instruct->instruct == '[' && p_curr_cell->value == 0) || (p_curr_instruct->instruct == ']' && p_curr_cell->value != 0))
		{
			p_curr_instruct = p_curr_instruct->p_matching_instruct;
		}
		else
		{
			p_curr_instruct = p_curr_instruct->p_next_instruct;
		}
	}

	return;
}

cell *initialize_ribbon()
{
	cell *p_first_cell = calloc(sizeof(cell), 1);
	cell *p_curr_cell = p_first_cell;
	int iterator;

	p_first_cell;
	for (iterator = 0; iterator < 1024; iterator++)
	{
		p_curr_cell->p_next_cell = calloc(sizeof(cell), 1);
		p_curr_cell->p_next_cell->p_prev_cell = p_curr_cell;
		p_curr_cell = p_curr_cell->p_next_cell;
	}
	p_curr_cell->p_next_cell = p_first_cell;
	p_first_cell->p_prev_cell = p_curr_cell;

	return p_first_cell;
}