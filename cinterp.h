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
#define ADD 0
#define SUB 1
#define MUL 2
#define DIV 3
//Conditions
#define GT	4
#define GTE 5
#define LT	6
#define LTE 7
#define EQ	8
#define NE	9
//Branch
#define IFEQ 10
#define JUMP 11
//Memory
#define PUSH 12
#define POP 13
#define LOAD 14
#define DUP 15
#define SWAP 16
//IO
#define PRINT 17

#define NUM_INSTRUCTIONS 10

static const cinstr instructions[] = 
{
	{PUSH, 10},
	{PUSH, 7},
	{PUSH, 23},
	{SWAP, 0},
	{POP, 0},
	{SWAP, 0},
	{PRINT, 0}
};


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

#define nom_pushnumber(interp, num) (push_number(interp, num))
#define nom_popnumber(interp, num) (pop_number(interp, num))

#endif