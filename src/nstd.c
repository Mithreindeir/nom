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
* 3. This notice may not be removed or altered from any source distribution.
*/

#include "nstd.h"
#include <math.h>
#include <time.h>
#include "napi.h"

#pragma warning(disable:4996)

//Standart Print Function
//Takes an arbitrary amount of arguments of any data type and prints it
void nom_print(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);

	//printf("args: %d\n", args);
	for (int i = 0; i < args; i++)
	{
		element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
		//printf("\t type: %d \n", e.type);
		if (e.type == NUM) {
			nom_number n = pop_number(currentframe->data_stack);
			if (floorf(n) == n)
				printf("%d", (int)n);
			else
				printf("%f", n);
		}
		else if (e.type == BOOL) {
			nom_boolean n = pop_bool(currentframe->data_stack);
			if (n)
				printf("True");
			else
				printf("False");
		}
		else if (e.type == STR)
		{
			nom_string str = pop_string(currentframe->data_stack);
			if (str.is_char) putchar(str.str[str.offset]);
			else printf("%s", str.str);
			gc_free(currentframe->gcol, str.str);
		}
		else if (e.type == STRUCT) {
			nom_struct ns = pop_struct(currentframe->data_stack);
			nom_print_struct(&ns);
			//nom_var_free_struct(currentframe, ns);
		}
	}
}
void nom_print_struct(nom_struct * ns)
{
	printf("[");
	for (int i=0; i < ns->num_members; i++) {
		nom_variable * v = ns->members[i];
		if (v->type == NUM) {
			nom_number n = *((nom_number*)v->value);
			if (floorf(n) == n)
				printf("%d", (int)n);
			else
				printf("%f", n);
		} else if (v->type == BOOL) {
			nom_boolean *n = v->value;
			if (*n)
				printf("True");
			else
				printf("False");
		} else if (v->type == STR) {
			nom_string * str = v->value;
			if (str->is_char) putchar(str->str[str->offset]);
			else printf("%s", str->str);
			//gc_free(currentframe->gcol, str.str);
		} else if (v->type == STRUCT || (v->num_members > 0)) {
			nom_struct mns;
			mns.members = v->members;
			mns.num_members = v->num_members;
			nom_print_struct(&mns);
		}
		if ((i+1) < ns->num_members) printf(", ");
	}
	printf("]");
}


//Returns input from stdin. Default as string unless number then return float or int
void nom_input(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);

	char str[256];
	memset(str, 0, 255);
	fgets(str, 255, stdin);

	int s = strlen(str);
	char * mstr = malloc(s+1);
	memcpy(mstr, str, s);
	mstr[s] = '\0';
	int len = 0;
	int type = token_type(mstr, &len);

	if (type == INT) {
		push_number(currentframe->data_stack, atoi(mstr));
		free(mstr);
	}
	else if (type == FLOAT) {
		push_number(currentframe->data_stack, atof(mstr));
		free(mstr);
	}
	else {
		mstr[s-1] = '\0';
		gc_add(currentframe->gcol, mstr);
		push_raw_string(currentframe->data_stack, mstr);
	}
}

//Seeds random function.
void nom_seed(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack); 

	if (currentframe->data_stack->num_elements < 1)
		return;
	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	if (e.type == NUM) {
		nom_number n;
		store(currentframe->data_stack, &n, sizeof(nom_number), 0);
		srand((int)n);
	}
}
//Returns a random number taking a min and a max
void nom_random(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	int min = (int)(pop_number(currentframe->data_stack));
	int max = (int)(pop_number(currentframe->data_stack));
	if (min==max) {
		push_number(currentframe->data_stack, 0);
		return;
	}
	int num =  (rand() % (max - min)) + min;
	push_number(currentframe->data_stack, num);
}
//Time since epoch
void nom_time(frame * currentframe)
{
	pop_number(currentframe->data_stack);
	float t = difftime(time(NULL), 0);
	push_number(currentframe->data_stack, t);
}

//CPU clock
void nom_clock(frame * currentframe)
{
	//pop_number(currentframe->data_stack);
	clock_t c = clock();
	float sec = ((float)c) / CLOCKS_PER_SEC;
	push_number(currentframe->data_stack, sec);
}

void nom_abs(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	push_number(currentframe->data_stack, abs(pop_number(currentframe->data_stack)));
}

void nom_floor(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	push_number(currentframe->data_stack, floor(pop_number(currentframe->data_stack)));
}

void nom_ceil(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	push_number(currentframe->data_stack, ceil(pop_number(currentframe->data_stack)));
}

void nom_reserve(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	//printf("\t type: %d \n", e.type);
	if (e.type == NUM) {
		//nom_number n = pop_number(currentframe->data_stack);
		//push_number(currentframe->data_stack, 1);
	}
	else if (e.type == BOOL) {
		//nom_boolean n = pop_bool(currentframe->data_stack);
		//push_number(currentframe->data_stack, 1);
	}
	else if (e.type == STR)
	{
		nom_string tstr = pop_string(currentframe->data_stack);
		int reserve = pop_number(currentframe->data_stack);

		if (tstr.num_characters == 0) {
			tstr.str = malloc(reserve+1);
			gc_add(currentframe->gcol, tstr.str);
			for(int i = 0; i < reserve; i++) {
				tstr.str[i] = 0;
			}
			tstr.str[reserve] = '\0';
			tstr.num_characters = reserve;
		} else {
			char * str = tstr.str;
			int ns = tstr.num_characters + reserve;
			tstr.str = realloc(tstr.str, ns);
			for(int i = tstr.num_characters-1; i < ns; i++) {
				tstr.str[i] = 0;
			}
			if (str != tstr.str) {
				gc_replace(currentframe->gcol, tstr.str, str);
			}
			tstr.num_characters += reserve;
		}
		push_string(currentframe->data_stack, tstr);
	}
	else if (e.type == STRUCT)
	{
		//nom_struct ns = pop_struct(currentframe->data_stack);
		//push_number(currentframe->data_stack, ns.num_members);
	}
}

void nom_size(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	//int args =1;
	if (args==0) {
		push_number(currentframe->data_stack, 0);
		return;
	}

	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	//printf("\t type: %d \n", e.type);
	if (e.type == NUM) {
		nom_number n = pop_number(currentframe->data_stack);
		push_number(currentframe->data_stack, 1);
	}
	else if (e.type == BOOL) {
		nom_boolean n = pop_bool(currentframe->data_stack);
		push_number(currentframe->data_stack, 1);
	}
	else if (e.type == STR)
	{
		nom_string tstr = pop_string(currentframe->data_stack);
		push_number(currentframe->data_stack, tstr.num_characters);
	}
	else if (e.type == STRUCT)
	{
		nom_struct ns = pop_struct(currentframe->data_stack);
		push_number(currentframe->data_stack, ns.num_members);
	}
}

void nom_string_init(frame * currentframe)
{
	pop_number(currentframe->data_stack);
	nom_string tstr;
	tstr.num_characters = 0;
	tstr.offset=0;
	tstr.str = NULL;
	tstr.is_char = 0;

	push_string(currentframe->data_stack, tstr);
}

void nom_array_init(frame * currentframe)
{
	pop_number(currentframe->data_stack);
	nom_struct ns;
	ns.members=NULL;
	ns.num_members=0;
	push_struct(currentframe->data_stack, ns);
}

//Runs a nom program taking PATH as input
void nom_run(frame * currentframe)
{

	int args = pop_number(currentframe->data_stack);

	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	if (e.type == STR)
	{
		nom_string str = pop_string(currentframe->data_stack);
		nom_run_file(str.str);
	}
}

void nom_open(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);

	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	if (e.type == STR)
	{
		nom_string str = pop_string(currentframe->data_stack);
		e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
		if (e.type != STR) {
			printf("2nd argument not a string\n");
			return;
		}
		nom_string mode = pop_string(currentframe->data_stack);
		char * buffer = NULL;
		int len;
		FILE * f = fopen(str.str, mode.str);
		if (f) {
			//Create struct to hold file
			nom_struct ns;
			ns.num_members = 1;
			ns.members = malloc(sizeof(nom_variable));
			nom_variable * var = malloc(sizeof(nom_variable));
			var->name = NULL;
			var->name = STRDUP("file");
			var->type = NONE;
			var->value = NULL;
			var->num_references = 1;
			var->num_members = 0;
			var->members = NULL;
			var->member_ref = 1;
			var->parent = NULL;
			var->external = f;
			ns.members[0] = var;
			gc_add(currentframe->gcol, var);
			gc_add(currentframe->gcol, var->name);
			gc_add(currentframe->gcol, ns.members);
			push_struct(currentframe->data_stack, ns);
		} else {
			printf("Error opening file\n");
			push_number(currentframe->data_stack, -1);
		}
	}
}

void nom_write(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	FILE * f=NULL;
	if (e.type == STRUCT) {
		nom_struct ns = pop_struct(currentframe->data_stack);
		int idx = -1;
		for (int i = 0; i < ns.num_members; i++) {
			if (!strcmp(ns.members[i]->name, "file")) {
				idx=i;
				break;
			}
		}
		if (idx==-1) {
			printf("Argument is not a file handle\n");
			return;
		}
		nom_variable * var = ns.members[idx];
		f = var->external;
	}
	if (!f) {
		printf("Error writing to file\n");
		return;
	}
	//printf("args: %d\n", args);
	for (int i = 0; i < args-1; i++)
	{
		e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
		//printf("\t type: %d \n", e.type);
		if (e.type == NUM) {
			nom_number n = pop_number(currentframe->data_stack);
			if (floorf(n) == n)
				fprintf(f, "%d", (int)n);
			else
				fprintf(f, "%f", n);
		}
		else if (e.type == BOOL) {
			nom_boolean n = pop_bool(currentframe->data_stack);
			if (n)
				fprintf(f, "True");
			else
				fprintf(f, "False");
		}
		else if (e.type == STR)
		{
			nom_string str = pop_string(currentframe->data_stack);
			if (str.is_char) {
				fputc(str.str[str.offset], f);
			}
			else fprintf(f, "%s", str.str);
			gc_free(currentframe->gcol, str.str);
		}
	}
}

void nom_close(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	if (e.type == STRUCT) {
		nom_struct ns = pop_struct(currentframe->data_stack);
		int idx = -1;
		for (int i = 0; i < ns.num_members; i++) {
			if (!strcmp(ns.members[i]->name, "file")) {
				idx=i;
				break;
			}
		}
		if (idx==-1) {
			printf("Argument is not a file handle\n");
			return;
		}
		nom_variable * var = ns.members[idx];
		FILE * f = var->external;
		fclose(f);
	}
}

void nom_read(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	if (e.type == STRUCT) {
		nom_struct ns = pop_struct(currentframe->data_stack);
		int idx = -1;
		for (int i = 0; i < ns.num_members; i++) {
			if (!strcmp(ns.members[i]->name, "file")) {
				idx=i;
				break;
			}
		}
		if (idx==-1) {
			printf("Argument is not a file handle\n");
			return;
		}
		nom_variable * var = ns.members[idx];
		FILE * fp = var->external;
		long size=0;
		fseek(fp, 0, SEEK_END); 
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET); 
		char * fstr = malloc(size);
		fread(fstr, 1, size, fp);
		fstr[size-1]='\0';
		gc_add(currentframe->gcol, fstr);
		push_raw_string(currentframe->data_stack, fstr);
	}
}
void nom_readline(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
	if (e.type == STRUCT) {
		nom_struct ns = pop_struct(currentframe->data_stack);
		int idx = -1;
		for (int i = 0; i < ns.num_members; i++) {
			if (!strcmp(ns.members[i]->name, "file")) {
				idx=i;
				break;
			}
		}
		if (idx==-1) {
			printf("Argument is not a file handle\n");
			return;
		}
		nom_variable * var = ns.members[idx];
		FILE * fp = var->external;
		char * line=NULL;
		size_t len=0;
		if (getline(&line, &len, fp)==-1) {
			char * ret = malloc(2);
			ret[0]='\n';
			ret[1]='\0';
			push_raw_string(currentframe->data_stack, ret);
			return;
		}
		gc_add(currentframe->gcol, line);
		push_raw_string(currentframe->data_stack, line);
	}
}