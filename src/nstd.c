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

#pragma warning(disable:4996)

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
			printf("%s", str.str);
		}
	}
}

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
	}
	else if (type == FLOAT) {
		push_number(currentframe->data_stack, atof(mstr));
	}
	else {
		push_raw_string(currentframe->data_stack, mstr);
	}
}

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

void nom_random(frame * currentframe)
{
	int args = pop_number(currentframe->data_stack);
	int min = (int)(pop_number(currentframe->data_stack));
	int max = (int)(pop_number(currentframe->data_stack));
	push_number(currentframe->data_stack, (rand() % (max - min)) + min);
}

void nom_time(frame * currentframe)
{
	pop_number(currentframe->data_stack);
	float t = difftime(time(NULL), 0);
	push_number(currentframe->data_stack, t);
}

void nom_clock(frame * currentframe)
{
	pop_number(currentframe->data_stack);
	int c = clock();
	float sec = (float)(c) / CLOCKS_PER_SEC;
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
