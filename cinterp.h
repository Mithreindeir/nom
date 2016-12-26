#ifndef CINTERP_H
#define CINTERP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//Stack size is 4kb
#define STACK_SIZE 4*1024


enum nom_types
{
	STR,
	NUM,
	BOOL,
	FUNC,
	NONE
};
typedef struct element element;

struct element
{
	element * next;
	char * data;
	int type;
	int size;
};

typedef float c_number;
typedef struct stack
{
	char buff[STACK_SIZE];
	element * elements;
	int num_elements;
	int base_ptr, stack_ptr;
} stack;


//CrapLang Instruction 1 per line
typedef struct cinstr
{
	int action;
	int operand;
} cinstr;

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

//Instructions to execute for example
static const cinstr instructions[] = 
{
	{PUSH, 0},
	{PUSH, 1},
	{PUSH, 1},
	{EQ, 0},
	{IFEQ, 8},
	{PUSH, 1},
	{ADD, 0},
	{PRINT, 0},
	{JUMP, 1}
};

/*
NOM:
while 1:
	print(1337)
end

WHILE COND:

END

GENERATED:
1: "SETUP CONDITION"
2: COMP COND
3: IFEQ 5
... code
4: JUMP 1
5: MORE CODE
*/

//NomLang interpreter
typedef struct cinterp
{
	stack data_stack;
	cinstr * instructions;
	int num_instructions;
	int instr_ptr;
} cinterp;


void execute(cinterp* cinterp);
void stack_init(stack * stk);

//Elements
void push_element(stack * stk, char * data, int size, int type);
void pop_element(stack * stk);

//Instructions
void push(stack * stk, void * val, int size_bytes);
void pop(stack * stk, int size_bytes);
void pop_store(stack * stk, int size_bytes, void * buf);
void store(stack * stk, void * buf, int size_bytes, int offset);
void load(stack * stk, void * val, int size_bytes, int offset);
void dup(stack * stk);
void swap(stack * stk);

void push_number(stack * Stk, c_number number);
c_number pop_number(stack * Stk);
//operations
void add(stack * stk);
void subtract(stack * stk);
void multiply(stack * stk);
void divide(stack * stk);
//Conditions
void gt(stack * stk);
void gte(stack * stk);
void lt(stack * stk);
void lte(stack * stk);
void eq(stack * stk);
void ne(stack * stk);

static void(*operation[4])(stack * stk) = {&add, &subtract, &multiply, &divide};
static void(*condition[6])(stack*stk) = {&gt, &gte, &lt, &lte, &eq, &ne};
#endif