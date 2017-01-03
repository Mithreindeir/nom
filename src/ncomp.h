#ifndef NCOMP_H
#define NCOMP_H

#include "ast.h"
#include "cinstr.h"
#include "cinterp.h"

typedef struct instr_list
{
	cinstr * instructions;
	int num_instructions;
} instr_list;

void push_instr(instr_list * instrl, int instr, float op);

void compile(node * bop, frame * currentframe);
void traverse(node * node);
void tree_traverse(node * node);

float solve_traverse(node * node);

void val_traverse(node * node, instr_list * instrl, frame * currentframe);

#endif