#ifndef CINTERP_H
#define CINTERP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cinstr.h"

//Stack size is 4kb
#define STACK_SIZE 4*1024

typedef enum nom_type
{
	STR,
	NUM,
	BOOL,
	FUNC,
	NONE
} nom_type;

typedef enum const_type
{
	STR_CONST,
	NUM_CONST,
	BOOL_CONST,
	FUNC_CONST,
	VAR_CONST,
	NONE_CONST
} const_type;

typedef double nom_number;
typedef int nom_boolean;
typedef void* nom_func;
typedef char* nom_string;

typedef struct nom_variable
{
	char * name;
	void * value;
	nom_type type;
	int num_references;
} nom_variable;



typedef struct element element;

struct element
{
	element * next;
	char * data;
	int type;
	int size;
};

typedef struct stack
{
	char buff[STACK_SIZE];
	element * elements;
	int num_elements;
	int base_ptr, stack_ptr;
} stack;

//Instructions to execute for example
static const cinstr instructions[] = 
{
	{PUSH, -1337},
	{STORE_NAME, 0},
	{PUSH, 5},
	{LOAD_NAME, 0},
	{PRINT, 0},
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

typedef struct frame
{
	stack data_stack;
	nom_variable * variables;
};

//NomLang interpreter
typedef struct cinterp
{
	stack data_stack;
	cinstr * instructions;
	int num_instructions;
	int instr_ptr;
	nom_variable * variables;
	int num_variables;
} cinterp;


int get_var_index(cinterp * cinterpreter, char * name);
void create_var(cinterp * cinterpreter, char * name, int type);

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

void push_number(stack * Stk, nom_number number);
nom_number pop_number(stack * Stk);
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