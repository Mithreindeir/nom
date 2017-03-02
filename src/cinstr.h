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

#ifndef CINSTR_H
#define CINSTR_H


//All possible opcodes for the nom vm
enum OPCODES
{
	//Operations
	ADD,	//Adds topmost elements and pushes result to stack
	SUB,	//Subtracts topmost 2 elements and pushes result to stack
	MUL,	//Multiplies topmost 2 elements and pushes result to stack
	DIV,	//Divides topmost 2 elements and pushes result to stack
	NEG,	//Negates the top of the stack and pushes it back
	AND,	//Ands topmost elements and pushes result to stack
	NOT,	//Nots topmost element and pushes result to stack
	OR,		//Ors topmost elements and pushes result to stack
	NOR,	//Norss topmost elements and pushes result to stack
	NAND,	//Nands topmost elements and pushes result to stack
	//Conditions
	GT,		//Compares topmost 2 with > and pushes result to stack
	GTE,	//Compares topmost 2 with >= and pushes result to stack
	LT,		//Compares topmost 2 with < and pushes result to stack
	LTE,	//Compares topmost 2 with <= and pushes result to stack
	EQ,		//Compares topmost 2 with == and pushes result to stack
	NE,		//Compares topmost 2 with != and pushes result to stack
	//Branch
	IFEQ,	//Jumps to x if top of stack is 0
	JUMP,	//Jumps to x
	//Memory
	PUSH_IDX,	//Pushes the idx to stack
	PUSH,	//Pushes number to stack
	POP,	//Pops top of stack
	PUSH_STR,	//Pushes string to stack
	POP_STR,	//Pops string of top of stack
	PUSH_FUNC,	//Pushes func to stack
	POP_FUNC,	//Pops func of top of stack
	LOAD,	//Loads top of stack to other element
	DUP,	//Duplicates top of stack
	SWAP,	//Swaps topmost 2 elements
	//IO
	PRINT,
	//Variable handling
	LOAD_NAME,	//Loads variable to top of stack
	STORE_NAME,	//Stores top of stack in variable
	//Functions
	CALL,	//Enters a new frame. Top of stack is num_args, and args are below it
	RET	//Returns from a function, and pushes return value (if there is one) to parent stack
};

//Nom Instruction. Has an index to a variable or constant array and an action
typedef struct cinstr
{
	int action;
	int idx;
} cinstr;

#endif