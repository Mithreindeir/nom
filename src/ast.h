#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include "clexer.h"
#include "nerr.h"

enum NODE_TYPE
{
	UNARY,
	BINARY,
	MULTI,
	LEAF,
	EMPTY
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
			node ** branches;
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

//Allocates a node and sets nothing
node * node_init();
//Creates a leaf
node * node_init_op(token num);
//Creates a node with a binary operator and two children
node * node_init_binary(token  op, node * l, node * r);
//Creates a node with a unary operator and a child
node * node_init_unary(token  op, node * n);
//Creates a root node and generates a ast for each line of code
node * parse_string(token * tokens, int num_tokens);
//Creates a block node and ends at if it reads an END token. Returns token after END
node * block_ast(token * tokens, int start, int num_tokens, int * tokens_used);
//Generates a ast from a line of code. Breaks on newline and sets tokens_used amount of tokens used
node * single_ast(token * tokens, int start, int num_tokens, int * tokens_used);
//Returns if the operator precedes the other operator, using their token
int precedes(token tok1, token tok2);
//Frees all memory in an AST
void free_nodes(node * root);

#endif