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

#include "cinterp.h"
#include <math.h>

#ifdef __linux__
#define STRDUP strdup
#elif _WIN32
#define STRDUP _strdup
#endif

nom_interp * nom_interp_init()
{
	nom_interp * nom = malloc(sizeof(nom_interp));
	nom->global_frame = frame_init();
	return nom;
}

void nom_interp_destroy(nom_interp * nom)
{
	exit_frame(nom->global_frame);
	free(nom);
}

frame *  frame_init()
{
	frame * cframe = malloc(sizeof(frame));
	cframe->parent = NULL;
	cframe->data_stack = stack_init();
	cframe->instructions = NULL;
	cframe->instr_ptr = 0;
	cframe->num_instructions = 0;
	cframe->variables = NULL;
	cframe->num_variables = 0;
	cframe->constants = NULL;
	cframe->num_constants = 0;

	return cframe;
}

//Copy constants and variables from a frame 
frame * frame_cpy(frame * original)
{
	frame * cframe = frame_init();
	/* TODO */
	return cframe;
}

void exit_frame(frame * frame)
{
	stack_destroy(frame->data_stack);

	for (int i = 0; i < frame->num_variables; i++)
	{
		//if (frame->variables[i].type == FUNC)
		//	exit_frame(((nom_func*)frame->variables[i].value)->frame);
		//if (frame->variables[i].type == STR)
		//	free(((nom_string*)frame->variables[i].value)->str);
		frame->variables[i].num_references--;
		free(frame->variables[i].name);

		if (frame->variables[i].num_references <= 0) {
			if (frame->variables[i].value) free(frame->variables[i].value);
		}
	}
	if (frame->variables) free(frame->variables);
	for (int i = 0; i < frame->num_constants; i++)
	{
		free(frame->constants[i]);
	}
	if (frame->constants) free(frame->constants);
	if (frame->instructions) free(frame->instructions);
	free(frame);
}

void destroy_frame(frame * frame)
{
}

int add_const(frame * frame, void * val)
{

	frame->num_constants++;
	if (frame->num_constants == 1)
	{
		frame->constants = malloc(sizeof(void*) * frame->num_constants);

	}
	else
	{
		frame->constants = realloc(frame->constants, sizeof(void*) * frame->num_constants);
	}
	
	int idx = frame->num_constants - 1;
	frame->constants[idx] = val;

	return idx;
}

void resize_string(nom_string * str, char * nstr, int size)
{
	if (str->num_characters != size)
	{
		if (str->num_characters == 0)
		{
			str->str = malloc(size + 1);
		}
		else str->str = realloc(str->str, size + 1);
	}
	for (int i = 0; i < size; i++)
	{
		str->str[i] = nstr[i];
	}
	str->str[size] = '\0';
}

void change_type(nom_variable * old, int ntype)
{
	if (old->type != ntype)
	{
		if (old->type == STR)
		{
			free(((nom_string*)old->value)->str);
		}
		if (old->type == NONE)
		{
			old->type = NUM;
			old->value = malloc(nom_type_size[ntype]);
		}
		else if (old->type != ntype)
		{
			old->type = NUM;
			free(old->value);
			old->value = malloc(nom_type_size[ntype]);
		}
	}
	old->type = ntype;
}

int get_var_index(frame * currentframe, char * name)
{
	if (currentframe == NULL)
		return -1;

	for (int i = 0; i < currentframe->num_variables; i++)
	{
		if (!strcmp(name, currentframe->variables[i].name))
			return i;
	}
	return -1;
}

void create_var(frame * currentframe, char * name, int type)
{
	nom_variable var;
	var.name = STRDUP(name);
	var.type = NONE;
	var.value = NULL;
	var.num_references = 1;

	currentframe->num_variables++;
	if (currentframe->num_variables == 1)
	{
		currentframe->variables = malloc(sizeof(nom_variable));
	}
	else
	{
		currentframe->variables = realloc(currentframe->variables, sizeof(nom_variable) * currentframe->num_variables);
	}
	currentframe->variables[currentframe->num_variables - 1] = var;
}

void execute(frame * currentframe)
{
	currentframe->instr_ptr = 0;
	while (currentframe->instr_ptr < currentframe->num_instructions)
	{
		cinstr c = currentframe->instructions[currentframe->instr_ptr];
		currentframe->instr_ptr++;
		if (c.action < 9)
			operation[c.action](currentframe->data_stack);
		else if (c.action < 15) {
			condition[c.action-9](currentframe->data_stack);
		}
		else if (c.action < 17)
		{
			if (c.action == IFEQ)
			{
				nom_number n;
				pop_store(currentframe->data_stack, sizeof(nom_number), &n);

				if (n == 0)
					currentframe->instr_ptr = c.idx;
			}
			else if (c.action == JUMP)
			{
				currentframe->instr_ptr = c.idx;
			}
		}
		else if (c.action == PUSH)
		{

			nom_number * num = currentframe->constants[c.idx];
			push_number(currentframe->data_stack, *num);
		}
		else if (c.action == POP)
		{
			pop(currentframe->data_stack, sizeof(nom_number));
		}
		else if (c.action == PUSH_STR)
		{
			char * str = currentframe->constants[c.idx];
			push_raw_string(currentframe->data_stack, str);
		}
		else if (c.action == POP_STR)
		{
			pop(currentframe->data_stack, sizeof(nom_string));
		}
		else if (c.action == PUSH_FUNC)
		{
			nom_func * func = currentframe->constants[c.idx];
			push_func(currentframe->data_stack, *func);
		}
		else if (c.action == POP_FUNC)
		{
			pop(currentframe->data_stack, sizeof(nom_func));
		}
		else if (c.action == LOAD)
		{
			nom_number n;
			store(currentframe->data_stack, &n, sizeof(nom_number), 0);
			load(currentframe->data_stack, &n, sizeof(nom_number), c.idx);
		}
		else if (c.action == DUP)
		{
			dup(currentframe->data_stack);
		}
		else if (c.action == SWAP)
		{
			swap(currentframe->data_stack);
		}
		else if (c.action == PRINT)
		{
			if (currentframe->data_stack->stack_ptr > 0)
			{
				element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
				if (e.type == NUM) {
					nom_number n;
					store(currentframe->data_stack, &n, sizeof(nom_number), 0);
					if (floorf(n) == n)
						printf("%d", (int)n);
					else
						printf("%f", n);
				}
				else if (e.type == BOOL) {
					nom_boolean n;
					store(currentframe->data_stack, &n, sizeof(nom_boolean), 0);
					if (n)
						printf("True");
					else
						printf("False");
				}
				else if (e.type == STR)
				{
					nom_string str;
					store(currentframe->data_stack, &str, sizeof(nom_string), 0);
					printf("%s", str.str);
				}
			}
		}
		else if (c.action == LOAD_NAME)
		{
			nom_variable * var = &currentframe->variables[(int)c.idx];
			if (var->type == NUM) {
				push_number(currentframe->data_stack, *((nom_number*)var->value));
			}
			else if (var->type == BOOL) {
				push_bool(currentframe->data_stack, *((nom_boolean*)var->value));
			}
			else if (var->type == STR)
			{
				push_string(currentframe->data_stack, *((nom_string*)var->value));
			}
			else if (var->type == FUNC)
			{
				push_number(currentframe->data_stack, 0);
			}
		}
		else if (c.action == STORE_NAME)
		{
			nom_variable * var = &currentframe->variables[(int)c.idx];
			element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
			change_type(&currentframe->variables[(int)c.idx], e.type);

			if (var->type == NUM) {
				*((nom_number*)var->value) = pop_number(currentframe->data_stack);
			}
			else if (var->type == BOOL) {
				*((nom_boolean*)var->value) = pop_bool(currentframe->data_stack);
			}
			else if (var->type == STR)
			{
				*((nom_string*)var->value) = pop_string(currentframe->data_stack);
			}
			else if (var->type == FUNC)
			{
				*((nom_func*)var->value) = pop_func(currentframe->data_stack);
			}
		}
		else if (c.action == CALL)
		{
			nom_variable * var = &currentframe->variables[(int)c.idx];
			nom_func f = *((nom_func*)var->value);
			if (f.external)
			{
				f.func(currentframe);
			}
			else {
				//Keep copy of frame
				frame * oldf = f.frame;
				//Make copy of mutable data and point to old immutable data for function
				// (So make new variables and stack)
				frame * newf = frame_init();
				//For recursive functions, this just alters the caller of the first times stack, not any of the other
				/* TODO */
				newf->parent = currentframe;
				newf->constants = oldf->constants;
				newf->num_constants = oldf->num_constants;
				newf->num_instructions = oldf->num_instructions;
				newf->instructions = oldf->instructions;

				for (int i = 0; i < oldf->num_variables; i++)
				{
					nom_variable v = oldf->variables[i];
					create_var(newf, v.name, NONE);
				}
				f.frame = newf;

				//Set top of stack to function args
				int args = pop_number(currentframe->data_stack);
				for (int i = 0; i < args; i++)
				{
					nom_variable * v = &f.frame->variables[(args - 1) - i];
					element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
					change_type(v, e.type);

					if (v->type == NUM) {
						*((nom_number*)v->value) = pop_number(currentframe->data_stack);
					}
					else if (v->type == BOOL) {
						*((nom_boolean*)v->value) = pop_bool(currentframe->data_stack);
					}
					else if (v->type == STR)
					{
						*((nom_string*)v->value) = pop_string(currentframe->data_stack);
					}
					else if (v->type == FUNC)
					{
						*((nom_func*)v->value) = pop_func(currentframe->data_stack);
					}
				}
				//Set variables in function to ones out of its scope
				for (int i = args; i < f.frame->num_variables; i++)
				{
					nom_variable * v = &f.frame->variables[i];
					nom_variable * rv = NULL;
					frame * fp = currentframe;
					while (fp)
					{
						int idx = get_var_index(fp, v->name);
						if (idx != -1)
							rv = &fp->variables[idx];
						fp = fp->parent;
					}
					if (rv)
					{
						//change_type(v, rv->type);
						v->num_references++;
						v->value = rv->value;
					}
				}
				execute(f.frame);
				newf->constants = NULL;
				newf->num_constants = 0;
				newf->num_instructions = 0;
				newf->instructions = NULL;
				exit_frame(newf);
				f.frame = oldf;
				//push_func(currentframe->data_stack, *((nom_func*)var->value));
			}
		}
		else if (c.action == RET)
		{
			if (currentframe->data_stack->stack_ptr > 0 && currentframe->parent)
			{
				element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
				if (e.type == NUM) {
					nom_number n;
					store(currentframe->data_stack, &n, sizeof(nom_number), 0);
					push_number(currentframe->parent->data_stack, n);
				}
				else if (e.type == BOOL) {
					nom_boolean n;
					store(currentframe->data_stack, &n, sizeof(nom_boolean), 0);
					push_bool(currentframe->parent->data_stack, n);

				}
				else if (e.type == STR)
				{
					nom_string str;
					store(currentframe->data_stack, &str, sizeof(nom_string), 0);
					push_string(currentframe->parent->data_stack, str);
				}
			}
			return;
		}
	}

}

stack * stack_init()
{
	stack * stk = malloc(sizeof(stack));
	stk->base_ptr = 0;
	stk->stack_ptr = 0;
	stk->buff = malloc(STACK_SIZE);
	for (int i = 0; i < STACK_SIZE; i++)
	{
		stk->buff[i] = 0;
	}
	stk->num_elements = 0;
	stk->elements = NULL;
	return stk;
}

void stack_destroy(stack * stack)
{
	if (stack->elements) free(stack->elements);
	free(stack->buff);
	free(stack);
}

void push_element(stack * stk, char * data, int size, int type)
{
	stk->num_elements++;
	if (stk->num_elements == 1)
	{
		stk->elements = malloc(sizeof(element));
	}
	else
	{
		stk->elements = realloc(stk->elements, sizeof(element) * stk->num_elements);
	}
	element e;
	e.data = data;
	e.size = size;
	e.type = type;
	stk->elements[stk->num_elements - 1] = e;
}

void pop_element(stack * stk)
{
	if (stk->num_elements > 0)
	{
		stk->num_elements--;
		stk->elements = realloc(stk->elements, sizeof(element) * stk->num_elements);
	}
}

void push(stack * stk, void * val, int size_bytes)
{
	char * cval = val;
	for (int i = 0; i < size_bytes && stk->stack_ptr < STACK_SIZE; i++)
	{
		stk->buff[stk->stack_ptr] = cval[i];
		stk->stack_ptr++;
	}
}

void  pop(stack * stk, int size_bytes)
{
	if (size_bytes > (stk->stack_ptr - stk->base_ptr))
		return;
	
	stk->stack_ptr -= size_bytes;
}

void pop_store(stack * stk, int size_bytes, void * buf)
{
	if (size_bytes > (stk->stack_ptr - stk->base_ptr))
		return;
	char * cbuf = buf;
	for (int i = 0; i < size_bytes && stk->stack_ptr < STACK_SIZE; i++)
	{
		cbuf[i] = stk->buff[stk->stack_ptr-size_bytes + i];
	}
	stk->stack_ptr -= size_bytes;
	pop_element(stk);
}

void store(stack * stk, void * buf, int size_bytes, int offset)
{
	if (size_bytes > (stk->stack_ptr - stk->base_ptr))
		return;
	char * cbuf = buf;
	for (int i = 0; i < size_bytes && stk->stack_ptr < STACK_SIZE; i++)
	{
		cbuf[i] = stk->buff[(stk->stack_ptr-offset) - size_bytes + i];
	}
}

void load(stack * stk, void * val, int size_bytes, int offset)
{
	char * cval = val;
	for (int i = 0; i < size_bytes && stk->stack_ptr < STACK_SIZE; i++)
	{
		stk->buff[stk->stack_ptr-offset] = cval[i];
		stk->stack_ptr++;
	}
}

void dup(stack * stk)
{
	if (stk->num_elements < 1)
		return;
	element e = stk->elements[stk->num_elements - 1];
	char * val = malloc(e.size);
	store(stk, val, e.size, 0);
	push(stk, val, e.size);
	push_element(stk, &stk->buff[stk->stack_ptr - e.size], e.size, e.type);
	free(val);
}

void swap(stack * stk)
{
	if (stk->num_elements < 2)
		return;
	element ea = stk->elements[stk->num_elements - 1], eb = stk->elements[stk->num_elements - 2];
	int sizea = ea.size, sizeb = eb.size;
	char * a = malloc(sizea);
	pop_store(stk, sizea, a);
	char * b = malloc(sizeb);
	pop_store(stk, sizeb, b);
	push(stk, a, sizea);
	push(stk, b, sizea);
	stk->elements[stk->num_elements - 1] = eb;
	stk->elements[stk->num_elements - 2] = ea;
}

void push_number(stack * stk, nom_number number)
{
	nom_number n = number;
	push(stk, &n, sizeof(nom_number));
	push_element(stk, &stk->buff[stk->stack_ptr - sizeof(nom_number)], sizeof(nom_number), NUM);
}

nom_number pop_number(stack * stk)
{
	nom_number n;
	pop_store(stk, sizeof(nom_number), &n);
	return n;
}

void push_bool(stack * stk, nom_boolean boolean)
{
	nom_boolean n = boolean;
	push(stk, &n, sizeof(nom_boolean));

	push_element(stk, &stk->buff[stk->stack_ptr - sizeof(nom_boolean)], sizeof(nom_boolean), BOOL);
}

nom_boolean pop_bool(stack * stk)
{
	nom_boolean n;
	pop_store(stk, sizeof(nom_boolean), &n);
	return n;
}

void push_string(stack * stk, nom_string str)
{
	push(stk, &str, sizeof(nom_string));
	push_element(stk, &stk->buff[stk->stack_ptr - sizeof(nom_string)], sizeof(nom_string), STR);
}

void push_raw_string(stack * stack, char * string)
{
	nom_string str;
	str.num_characters = 0;
	str.str = string;
	//resize_string(&str, string, strlen(string));
	push(stack, &str, sizeof(nom_string));
	push_element(stack, &stack->buff[stack->stack_ptr - sizeof(nom_string)], sizeof(nom_string), STR);
}

nom_string pop_string(stack * stk)
{
	nom_string str;
	pop_store(stk, sizeof(nom_string), &str);
	return str;
}

void push_func(stack * stack, nom_func func)
{
	push(stack, &func, sizeof(nom_func));
	push_element(stack, &stack->buff[stack->stack_ptr - sizeof(nom_func)], sizeof(nom_func), FUNC);
}

nom_func pop_func(stack * stk)
{
	nom_func func;
	pop_store(stk, sizeof(nom_func), &func);
	return func;
}

void add(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b + a);
}

void subtract(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b - a);
}

void multiply(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b * a);
}

void divide(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b / a);
}

void negate(stack * stk)
{
	nom_number a = pop_number(stk);
	push_number(stk, -a);
}

void gt(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b > a);
}

void gte(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b >= a);
}

void lt(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b < a);
}

void lte(stack * stk)
{

	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b <= a);

}

void eq(stack * stk)
{

	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b == a);
}

void ne(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b != a);
}

void and(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b && a);
}

void nand(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, !(b && a));
}

void or (stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b || a);
}

void nor(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, !(b || a));
}
