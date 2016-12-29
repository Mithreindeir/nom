#ifndef CINSTR_H
#define CINSTR_H

enum OPCODES
{
	//Operations
	ADD,	//Adds topmost elements and pushes result to stack
	SUB,	//Subtracts topmost 2 elements and pushes result to stack
	MUL,	//Multiplies topmost 2 elements and pushes result to stack
	DIV,	//Divides topmost 2 elements and pushes result to stack
	NEG,	//Negates the top of the stack and pushes it back
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
	PUSH,	//Pushes number to stack
	POP,	//Pops top of stack
	LOAD,	//Loads top of stack to other element
	DUP,	//Duplicates top of stack
	SWAP,	//Swaps topmost 2 elements
	//IO
	PRINT,
	//Variable handling
	LOAD_NAME,	//Loads variable to top of stack
	STORE_NAME,	//Stores top of stack in variable
};

//CrapLang Instruction 1 per line
typedef struct cinstr
{
	int action;
	float operand;
} cinstr;

#endif