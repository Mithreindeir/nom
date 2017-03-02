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

#ifndef NCOMP_H
#define NCOMP_H

#include "ast.h"
#include "cinstr.h"
#include "cinterp.h"

//List of instructions to be added to interpreter
typedef struct instr_list
{
	cinstr * instructions;
	int num_instructions;
} instr_list;


//Creates and adds an instruction to the list
void push_instr(instr_list * instrl, int instr, int op);

//Compiles an AST and creates a frame around it 
void compile(node * bop, frame * currentframe);
//Pushes indexes of members for correct reference
int push_member_idx(node * node, instr_list * instrl, frame * currentframe);
//Traverses an AST, and prints values
void traverse(node * node);
//Traverses an AST and compiles recursively
void val_traverse(node * node, instr_list * instrl, frame * currentframe);

#endif