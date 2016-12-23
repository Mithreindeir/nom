#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>

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
	char * val;
};

num num_init(char * tok);
binop * binop_init_op(char * num);
binop * binop_init_op(char * op, binop * l, binop * r);
void parse_string(char * string);

#endif