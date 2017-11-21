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
				printf("%ld", (int)n);
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
	}
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
	push_number(currentframe->data_stack, (rand() % (max - min)) + min);
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
}

void nom_floor(frame * currentframe)
{
}

void nom_ceil(frame * currentframe)
{
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
			tstr.str = malloc(reserve);
			gc_add(currentframe->gcol, tstr.str+1);
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
