#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cinterp.h"
#include "clexer.h"
#include "ast.h"
#include "ncomp.h"

char string[] = "5 * -3.5";

cinterp cinterpreter;

int main()
{
	cinterpreter.variables = NULL;
	cinterpreter.num_variables = 0;

	int num_tokens = 0;
	token * tokens = tokenize(string, &num_tokens);
	binop * bop = parse_string(tokens, num_tokens);
	
	instr_list * l = compile(bop, &cinterpreter);
	cinterpreter.instructions = l->instructions;
	cinterpreter.num_instructions = l->num_instructions;

	for (int i = 0; i < num_tokens; i++)
	{
		//printf("token: %s\t type: %d\n", tokens[i].tok, tokens[i].type);
		free(tokens[i].tok);
	}
	free(tokens);
	//getch();
	//return;
	stack_init(&cinterpreter.data_stack);
	execute(&cinterpreter);
	getch();
	return;
}