#ifndef CINTERP_H
#define CINTERP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cinstr.h"
#include "nerr.h"

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

typedef float nom_number;
typedef int nom_boolean;
typedef void* nom_func;
typedef struct nom_string
{
	int num_characters;
	char  * str;
} nom_string;

const static int nom_type_size[] = { sizeof(nom_string), sizeof(nom_number), sizeof(nom_boolean), sizeof(nom_func), 0 };

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

typedef struct frame
{
	stack data_stack;
	cinstr * instructions;
	int num_instructions;
	int instr_ptr;
	nom_variable * variables;
	int num_variables;
} frame;

//NomLang interpreter
typedef struct nom_interp
{
	frame * global_frame;
} nom_interp;

//Interpreter functions
nom_interp * nom_interp_init();
void nom_interp_destroy(nom_interp * nom);

//Frame functions
void exit_frame(frame * frame);
void destroy_frame(frame * frame);

//Variable functions
void resize_string(nom_string * str, char * nstr, int size);
void change_type(nom_variable * old, int ntype);
int get_var_index(frame * currentframe, char * name);
void create_var(frame * currentframe, char * name, int type);

void execute(frame * currentframe);
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
void push_bool(stack * stk, nom_boolean boolean);
nom_boolean pop_bool(stack * stk);
void push_string(stack * stk, nom_string str);
void push_raw_string(stack * stack, char * str);
nom_string pop_string(stack * stk);
//operations
void add(stack * stk);
void subtract(stack * stk);
void multiply(stack * stk);
void divide(stack * stk);
void negate(stack * stk);
//Conditions
void gt(stack * stk);
void gte(stack * stk);
void lt(stack * stk);
void lte(stack * stk);
void eq(stack * stk);
void ne(stack * stk);
//Logical operations
void and(stack * stk);
void nand(stack * stk);
void or(stack * stk);
void nor(stack * stk);

static void(*operation[9])(stack * stk) = {&add, &subtract, &multiply, &divide, &negate, &and, &or, &nor, &nand};
static void(*condition[6])(stack*stk) = {&gt, &gte, &lt, &lte, &eq, &ne};
#endif