#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cinterp.h"
#include "clexer.h"
#include "ast.h"
#include "ncomp.h"

char string[] =
"x = 5*1\n"
"x = x * x\n"
"x = 5 * 4\n"
"y = x * 4 + 3.0/(2 + 4)\n"
"y\n";

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

	for (int i = 0; i < cinterpreter.num_variables; i++)
	{
		free(cinterpreter.variables[i].value);
	}
	if (cinterpreter.variables) free(cinterpreter.variables);
	getch();
	return;
}