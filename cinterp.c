#include "cinterp.h"

void execute(cinterp * cinterp)
{
	cinterp->instr_ptr = 0;
	while (cinterp->instr_ptr < (sizeof(instructions)/sizeof(cinstr)))
	{
		cinstr c = instructions[cinterp->instr_ptr];
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

				c_number n;
				pop_store(&cinterp->data_stack, sizeof(c_number), &n);
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
			push_number(&cinterp->data_stack, c.operand);
		}
		else if (c.action == POP)
		{
			pop(&cinterp->data_stack, sizeof(c_number));
		}
		else if (c.action == LOAD)
		{
			c_number n;
			store(&cinterp->data_stack, &n, sizeof(c_number), 0);
			load(&cinterp->data_stack, &n, sizeof(c_number), c.operand);
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
			c_number n;
			store(&cinterp->data_stack, &n, sizeof(c_number), 0);
			printf("%f\n", n);
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

void push_number(stack * stk, c_number number)
{
	c_number n = number;
	push(stk, &n, sizeof(c_number));
	push_element(stk, stk->buff[stk->stack_ptr - sizeof(c_number)], sizeof(c_number), NUM);
}

c_number pop_number(stack * stk)
{
	c_number n;
	pop_store(stk, sizeof(c_number), &n);
	pop_element(stk);
	return n;
}

void add(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a + b);
}

void subtract(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a - b);
}

void multiply(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a * b);
}

void divide(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a / b);
}

void gt(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a > b);
}

void gte(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a >= b);
}

void lt(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a < b);
}

void lte(stack * stk)
{

	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a <= b);

}

void eq(stack * stk)
{

	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a == b);
}

void ne(stack * stk)
{
	c_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, a != b);
}
