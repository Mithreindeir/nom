#include "cinterp.h"

int get_var_index(cinterp * cinterpreter, char * name)
{
	for (int i = 0; i < cinterpreter->num_variables; i++)
	{
		if (!strcmp(name, cinterpreter->variables[i].name))
			return i;
	}
	return -1;
}

void create_var(cinterp * cinterpreter, char * name, int type)
{
	nom_variable var;
	var.name = name;
	var.type = type;
	if (var.type == NUM)
	{
		var.value = malloc(sizeof(nom_number));
		*((nom_number*)var.value) = 0;
	}

	cinterpreter->num_variables++;
	if (cinterpreter->num_variables == 1)
	{
		cinterpreter->variables = malloc(sizeof(nom_variable) * cinterpreter->num_variables);

	}
	else
	{
		cinterpreter->variables = realloc(cinterpreter->variables, sizeof(nom_variable) * cinterpreter->num_variables);
	}
	cinterpreter->variables[cinterpreter->num_variables - 1] = var;
}

void execute(cinterp * cinterp)
{
	cinterp->instr_ptr = 0;
	while (cinterp->instr_ptr < cinterp->num_instructions)
	{
		cinstr c = cinterp->instructions[cinterp->instr_ptr];
		cinterp->instr_ptr++;
		if (c.action < 4)
			operation[c.action](&cinterp->data_stack);
		else if (c.action < 10) {
			condition[c.action-4](&cinterp->data_stack);
		}
		else if (c.action < 12)
		{
			if (c.action == IFEQ)
			{

				nom_number n;
				pop_store(&cinterp->data_stack, sizeof(nom_number), &n);
				if (n == 0)
					cinterp->instr_ptr = c.operand;

			}
			else if (c.action == JUMP)
			{
				cinterp->instr_ptr = c.operand;
			}
		}
		else if (c.action == PUSH)
		{
			push_number(&cinterp->data_stack, (nom_number)c.operand);
		}
		else if (c.action == POP)
		{
			pop(&cinterp->data_stack, sizeof(nom_number));
		}
		else if (c.action == LOAD)
		{
			nom_number n;
			store(&cinterp->data_stack, &n, sizeof(nom_number), 0);
			load(&cinterp->data_stack, &n, sizeof(nom_number), c.operand);
		}
		else if (c.action == DUP)
		{
			dup(&cinterp->data_stack);
		}
		else if (c.action == SWAP)
		{
			swap(&cinterp->data_stack);
		}
		else if (c.action == PRINT)
		{
			if (cinterp->data_stack.stack_ptr > 0)
			{
				nom_number n;
				store(&cinterp->data_stack, &n, sizeof(nom_number), 0);
				printf("%f", n);
			}
		}
		else if (c.action == LOAD_NAME)
		{
			int nbytes = 0;
			nom_variable var = cinterp->variables[c.operand];
			if (var.type == NUM) 
				nbytes = sizeof(nom_number);
			push_number(&cinterp->data_stack, *((nom_number*)var.value));
		}
		else if (c.action == STORE_NAME)
		{
			pop_store(&cinterp->data_stack, sizeof(nom_number), cinterp->variables[c.operand].value);
		}
	}

}

void stack_init(stack * stk)
{
	stk->base_ptr = 0;
	stk->stack_ptr = 0;
	for (int i = 0; i < STACK_SIZE; i++)
	{
		stk->buff[i] = 0;
	}
	stk->num_elements = 0;
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
		return NULL;
	
	stk->stack_ptr -= size_bytes;
}

void pop_store(stack * stk, int size_bytes, void * buf)
{
	if (size_bytes > (stk->stack_ptr - stk->base_ptr))
		return NULL;
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
		return NULL;
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
	push_element(stk, stk->buff[stk->stack_ptr - e.size], e.size, e.type);
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
	push_element(stk, stk->buff[stk->stack_ptr - sizeof(nom_number)], sizeof(nom_number), NUM_CONST);
}

nom_number pop_number(stack * stk)
{
	nom_number n;
	pop_store(stk, sizeof(nom_number), &n);
	pop_element(stk);
	return n;
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
