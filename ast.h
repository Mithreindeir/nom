#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include "clexer.h"

typedef struct num
{
	int value;
	char * num;
} num;

typedef struct binop binop;

struct binop
{
	binop * left;
	binop * right;
	token val;
};

typedef struct op_stack
{
	binop ** stack;
	int size;
} op_stack;

op_stack * op_stack_init();
binop * op_stack_gettop(op_stack * stack);
void op_stack_push(op_stack * stack, binop * binop);
binop * op_stack_pop(op_stack * stack);

num num_init(char * tok);
binop * binop_init();
binop * binop_init_op(token num);
binop * binop_init_branch(token  op, binop * l, binop * r);
binop * parse_string(token * tokens, int num_tokens);

#endif