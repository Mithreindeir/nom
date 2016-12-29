#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include "clexer.h"

enum NODE_TYPE
{
	UNARY,
	BINARY,
	MULTI,
	LEAF
};

typedef struct binop binop;

struct binop
{
	binop * left;
	binop * right;
	token val;
};

typedef struct node node;

struct node
{
	int type;
	union {
		struct
		{
			node * left;
			node * right;
		};
		node * next;
		struct
		{
			node * branches;
			int num_branches;
		};
	};
	token val;
};

typedef struct op_stack
{
	node ** stack;
	int size;
} op_stack;

op_stack * op_stack_init();
node * op_stack_gettop(op_stack * stack);
void op_stack_push(op_stack * stack, node * node);
node * op_stack_pop(op_stack * stack);

node * node_init();
node * node_init_op(token num);
node * node_init_binary(token  op, node * l, node * r);
node * node_init_unary(token  op, node * n);
node * parse_string(token * tokens, int num_tokens);
int precedes(token tok1, token tok2);

#endif