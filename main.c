#define _CRT_SECURE_NO_DEPRECATE 1
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cinterp.h"
#include "clexer.h"
#include "ast.h"
#include "ncomp.h"

nom_interp * nom;

int main()
{
	getch();
	char * buffer = 0;
	long length;
	FILE * f = fopen("test.nom", "r");
	nom = nom_interp_init();
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length+1);
		memset(buffer, 0, length + 1);
		if (buffer)
		{
			fread(buffer, 1, length, f);
		}
		fclose(f);
		buffer[length] = '\0';
	}
	if (buffer)
	{

		int num_tokens = 0;
		token * tokens = tokenize(buffer, &num_tokens);
		binop * bop = parse_string(tokens, num_tokens);
		instr_list * l = compile(bop, nom->global_frame);
		nom->global_frame->instructions = l->instructions;
		nom->global_frame->num_instructions = l->num_instructions;
		free(l);
		getch();
		for (int i = 0; i < num_tokens; i++)
		{
			//printf("token: %s\t type: %d\n", tokens[i].tok, tokens[i].type);
			free(tokens[i].tok);
		}
		free(tokens);
		//return;
		stack_init(&nom->global_frame->data_stack);
		execute(nom->global_frame);
	}
	free(buffer);
	nom_interp_destroy(nom);
	getch();

	return;
}
