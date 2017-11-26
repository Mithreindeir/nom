
/*
* Copyright (c) 2016 Cormac Grindall (Mithreindeir)
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* vrFreely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution->
*/

#include "napi.h"
#include <time.h>


void nom_run_file(char * file)
{
	
	nom_interp * nom;
	char * buffer = 0;
	long length;
	FILE * f = fopen(file, "r+");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length + 1);
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
		nom = nom_interp_init();
		nom_register_func(nom, "print", &nom_print, 1);
		nom_register_func(nom, "import", &nom_import_wrapper, 1);
		nom_register_func(nom, "input", &nom_input, 0);
		nom_register_func(nom, "time", &nom_time, 0);
		nom_register_func(nom, "clock", &nom_clock, 0);
		nom_register_func(nom, "seed", &nom_seed, 1);
		nom_register_func(nom, "random", &nom_random, 2);
		nom_register_func(nom, "floor", &nom_floor, 1);
		nom_register_func(nom, "ceil", &nom_ceil, 1);
		nom_register_func(nom, "abs", &nom_abs, 1);
		nom_register_func(nom, "run", &nom_run, 1);
		nom_register_func(nom, "string", &nom_string_init, 0);
		nom_register_func(nom, "array", &nom_array_init, 0);
		nom_register_func(nom, "size", &nom_size, 1);
		nom_register_func(nom, "reserve", &nom_reserve, 2);
		nom_register_func(nom, "open", &nom_open, 2);
		nom_register_func(nom, "close", &nom_close, 1);
		nom_register_func(nom, "write", &nom_write, 1);
		nom_register_func(nom, "read", &nom_read, 1);
		nom_register_func(nom, "readline", &nom_readline, 1);


		int num_tokens = 0;
		token * tokens = tokenize(buffer, &num_tokens);
		node * bop = parse_string(tokens, num_tokens);
		compile(bop, nom->global_frame);
		//getch();
		for (int i = 0; i < num_tokens; i++)
		{
			//printf("token: %s\t type: %d\n", tokens[i].tok, tokens[i].type);
			free(tokens[i].tok);
		}
		free(tokens);
		//return;
		//stack_init(&nom->global_frame->data_stack);
		execute(nom->global_frame);
		free(buffer);
		nom_interp_destroy(nom);
	}
}

void nom_import_file(frame * cf, char * file)
{
	char * buffer = 0;
	long length;
	FILE * f = fopen(file, "r+");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length + 1);
		memset(buffer, 0, length + 1);
		if (buffer)
		{
			fread(buffer, 1, length, f);
		}
		fclose(f);
		buffer[length] = '\0';
	}
	frame * frame = frame_init();
	frame->parent = NULL;
	//frame_add_child(cf, frame);
	frame->gcol=cf->gcol;
	if (buffer)
	{
		int n = frame->num_variables;
		int num_tokens = 0;
		token * tokens = tokenize(buffer, &num_tokens);
		node * bop = parse_string(tokens, num_tokens);
		compile(bop, frame);
		execute(frame);
		for (int i = 0; i < num_tokens; i++)
		{
			//printf("token: %s\t type: %d\n", tokens[i].tok, tokens[i].type);
			free(tokens[i].tok);
		}
		free(tokens);
		free(buffer);
		for(int i=n; i<frame->num_variables;i++) {
			nom_set_var(cf, frame->variables[i]);
			nom_var_add_ref(cf, frame->variables[i]);
			frame->variables[i] = NULL;
		}
		for (int i = 0; i < frame->num_children; i++) {
			if (frame->children[i] != frame) {
				frame->children[i]->parent = cf;
				frame_add_child(cf, frame->children[i]);
			}
		}
		free(frame->children);
		frame->children = NULL;
		frame->num_children = 0;
		frame->gcol = NULL;
		exit_frame(frame);
	}
}

void nom_set_var(frame * cf, nom_variable * n)
{
	if (!cf) return;
	for (int i = 0; i < cf->num_children; i++) {
		nom_set_var(cf->children[i], n);
	}
	int idx = get_var_index(cf, n->name);
	if (idx != -1) {
		cf->variables[idx]=n;
	}
	else {
		create_var(cf, n->name, n->type);
		gc_free(cf->gcol, cf->variables[cf->num_variables - 1]->name);
		gc_free(cf->gcol, cf->variables[cf->num_variables - 1]);
		cf->variables[cf->num_variables - 1]=n;
	}
}

void nom_import_wrapper(frame * cf)
{
	int args = pop_number(cf->data_stack);
	element e = cf->data_stack->elements[cf->data_stack->num_elements - 1];
	if (e.type == STR) {
		nom_string s = pop_string(cf->data_stack);
		nom_import_file(cf, s.str);
	}	
}

void nom_repl()
{
	// Don't use this 
	nom_interp * nom;
	nom = nom_interp_init();
	nom_register_func(nom, "print", &nom_print, 1);
	nom_register_func(nom, "import", &nom_import_wrapper, 1);
	nom_register_func(nom, "input", &nom_input, 0);
	nom_register_func(nom, "time", &nom_time, 0);
	nom_register_func(nom, "clock", &nom_clock, 0);
	nom_register_func(nom, "seed", &nom_seed, 1);
	nom_register_func(nom, "random", &nom_random, 2);
	nom_register_func(nom, "floor", &nom_floor, 1);
	nom_register_func(nom, "ceil", &nom_ceil, 1);
	nom_register_func(nom, "abs", &nom_abs, 1);
	nom_register_func(nom, "run", &nom_run, 1);
	nom_register_func(nom, "string", &nom_string_init, 0);
	nom_register_func(nom, "array", &nom_array_init, 0);
	nom_register_func(nom, "size", &nom_size, 1);
	nom_register_func(nom, "reserve", &nom_reserve, 2);
	nom_register_func(nom, "open", &nom_open, 2);
	nom_register_func(nom, "close", &nom_close, 1);
	nom_register_func(nom, "write", &nom_write, 1);
	nom_register_func(nom, "read", &nom_read, 1);
	nom_register_func(nom, "readline", &nom_readline, 1);


	printf(">");
	char buff[256];
	memset(buff, 0, 255);
	fgets(buff, 255, stdin);
	int scompile = 1;
	stack_init(&nom->global_frame->data_stack);

	while (strncmp(buff, "exit()", 4))
	{
		int blocks = 0;
		int num_tokens = 0;
		//printf("%s\n", buff);
		//getch();
		token * tokens = tokenize(buff, &num_tokens);
		for (int i = 0; i < num_tokens; i++)
		{
			token tok = tokens[i];
			if (tok.type == COLON) {
				blocks++;
			}
			if (tok.type == END) {
				blocks--;
			}
		}
		if (blocks == 0)
			scompile = 1;
		else
			scompile = 0;

		if (scompile)
		{

			node * bop = parse_string(tokens, num_tokens);
			compile(bop, nom->global_frame);

			//getch();
			for (int i = 0; i < num_tokens; i++)
			{
				//printf("token: %s\t type: %d\n", tokens[i].tok, tokens[i].type);
				free(tokens[i].tok);
			}
			free(tokens);
			//return;
			execute(nom->global_frame);
		}

		if (scompile)
		{
			printf("\n>");
			memset(buff, 0, 255);
			fgets(buff, 255, stdin);
		}
		else
		{
			int size = strlen(buff);
			fgets(buff + size, 255 - size, stdin);
		}
	}
	nom_interp_destroy(nom);

}

void nom_register_func(nom_interp * nom, char * name, nom_external_func func, int args)
{
	int idx = get_var_index(nom->global_frame, name);
	if (idx == -1)
	{
		create_var(nom->global_frame, name, FUNC);
		idx = nom->global_frame->num_variables - 1;
	}
	nom_func * nfunc = malloc(sizeof(nom_func));
	gc_add(nom->global_frame->gcol, nfunc);
	nfunc->external = 1;
	nfunc->func = func;
	nfunc->arg_count = args;
	nom_variable * nf = nom->global_frame->variables[idx];
	nf->value = nfunc;
	nf->type = FUNC;

}
void nom_register_func_frame(frame * f, char * name, nom_external_func func, int args)
{
	int idx = get_var_index(f, name);
	if (idx == -1)
	{
		create_var(f, name, FUNC);
		idx =f->num_variables - 1;
	}
	nom_func * nfunc = malloc(sizeof(nom_func));
	gc_add(f->gcol, nfunc);
	nfunc->external = 1;
	nfunc->func = func;
	nfunc->arg_count = args;
	nom_variable * nf = f->variables[idx];
	nf->value = nfunc;
	nf->type = FUNC;
}