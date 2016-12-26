#ifndef CINSTR_H
#define CINSTR_H

//Operations
#define ADD 0	//Adds topmost elements and pushes result to stack
#define SUB 1	//Subtracts topmost 2 elements and pushes result to stack
#define MUL 2	//Multiplies topmost 2 elements and pushes result to stack
#define DIV 3	//Divides topmost 2 elements and pushes result to stack
//Conditions
#define GT	4	//Compares topmost 2 with > and pushes result to stack
#define GTE 5	//Compares topmost 2 with >= and pushes result to stack
#define LT	6	//Compares topmost 2 with < and pushes result to stack
#define LTE 7	//Compares topmost 2 with <= and pushes result to stack
#define EQ	8	//Compares topmost 2 with == and pushes result to stack
#define NE	9	//Compares topmost 2 with != and pushes result to stack
//Branch
#define IFEQ 10		//Jumps to x if top of stack is 0
#define JUMP 11		//Jumps to x
//Memory
#define PUSH 12		//Pushes number to stack
#define POP 13		//Pops top of stack
#define LOAD 14		//Loads top of stack to other element
#define DUP 15		//Duplicates top of stack
#define SWAP 16		//Swaps topmost 2 elements
//IO
#define PRINT 17
//Variable handling
#define LOAD_NAME 18	//Loads variable to top of stack
#define STORE_NAME 19	//Stores top of stack in variable

//CrapLang Instruction 1 per line
typedef struct cinstr
{
	int action;
	int operand;
} cinstr;

#endif