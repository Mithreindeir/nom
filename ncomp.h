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

instr_list * compile(binop * bop, cinterp * cinterpreter);
void traverse(binop * binop);
void tree_traverse(binop * binop);

float solve_traverse(binop * binop);

void val_traverse(binop * binop, instr_list * instrl, cinterp * cinterpreter);

#endif