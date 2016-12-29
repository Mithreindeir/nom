#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include "clexer.h"

typedef struct unaryop
{
	unaryop * next;
	token val;
} unaryop;

typedef struct binop
{
	binop * left;
	binop * right;
	token val;
} binop;

typedef struct multiop
{
	multiop ** nodes;
	int num_nodes;
	token val;
} multiop;

typedef struct op_stack
{
	binop ** stack;
	int size;
} op_stack;

typedef struct node
{
	union
	{
		unaryop * unary_node;
		binop * binary_node;
		multiop * infiniteop;
	};
} node;

op_stack * op_stack_init();
binop * op_stack_gettop(op_stack * stack);
void op_stack_push(op_stack * stack, binop * binop);
binop * op_stack_pop(op_stack * stack);

binop * binop_init();
binop * binop_init_op(token num);
binop * binop_init_branch(token  op, binop * l, binop * r);
binop * parse_string(token * tokens, int num_tokens);
int precedes(token tok1, token tok2);

#endif