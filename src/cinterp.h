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

#ifndef CINTERP_H
#define CINTERP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cinstr.h"
#include "nerr.h"
#include "gc.h"

//Stack chunk is 512 bytes
#define STACK_CHUNK 256
//Epsilon for floating point comparing
#define EPSILON 0.000001


typedef enum nom_type
{
	STR,
	NUM,
	BOOL,
	FUNC,
	STRUCT,
	NONE
} nom_type;

typedef struct nom_variable nom_variable;

//Variable in nom
struct nom_variable
{
	char * name;
	void * value;
	nom_type type;
	int num_references;
	int num_members;
	int member_ref;
	nom_variable * members;
	nom_variable * parent;
};

typedef struct element element;
//Element in the data stack. 
struct element
{
	element * next;
	char * data;
	int type;
	int size;
};

//Data stack used in stack machine as intermediary between values, variables, and operations
typedef struct stack
{
	char * buff;

	element * elements;
	int num_elements;
	int chunks;
	int base_ptr, stack_ptr;
} stack;

typedef struct frame frame;
//Program frame. This holds all information about a program in a certain scope
struct frame
{
	frame ** children;
	frame * parent;
	stack * data_stack;
	cinstr * instructions;
	int num_instructions;
	int instr_ptr;
	nom_variable * variables;
	int num_variables;
	void ** constants;
	int num_constants;
	int num_children;
	gc * gcol;
};

typedef double nom_number;
typedef int nom_boolean;
typedef void(*nom_external_func)(frame * cf);
//Callable function in nom
typedef struct nom_func
{
	frame * frame;
	frame * global;
	int arg_count;
	int external;
	nom_external_func func;
} nom_func;
//String data type 
typedef struct nom_string
{
	int num_characters;
	int is_char;
	int offset;
	char  * str;
} nom_string;

//User struct type (temporary for passing)
typedef struct nom_struct
{
	int mem_ref;
	int num_members;
	nom_variable * members;
} nom_struct;

const static int nom_type_size[] = { sizeof(nom_string), sizeof(nom_number), sizeof(nom_boolean), sizeof(nom_func), 0 };

//NomLang interpreter
typedef struct nom_interp
{
	frame * global_frame;
} nom_interp;


//Interpreter functions
nom_interp * nom_interp_init();
void nom_interp_destroy(nom_interp * nom);


//Frame functions
frame * frame_init();
frame * frame_cpy(frame * original);
void frame_add_child(frame * parent, frame * child);
void exit_frame(frame * frame);
void destroy_frame(frame * frame);

//Constant functions
int add_const(frame * frame, void * val);

//Variable functions
void resize_string(nom_string * str, char * nstr, int size);
void change_type(frame * frame, nom_variable * old, int ntype);
int get_var_index(frame * currentframe, char * name);
void create_var(frame * currentframe, char * name, int type);
//Add or get from a variables member list
int get_var_index_local(nom_variable * lvar, char * name);
void create_var_local(nom_variable * lvar,  char * name, int type);
void nom_var_free_members(frame * frame, nom_variable * var);
void nom_var_add_ref(frame * frame, nom_variable * var);


void execute(frame * currentframe);
stack * stack_init();
void stack_resize(stack * stack, int size_bytes);
void stack_destroy(stack * stack);

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
void push_func(stack * stack, nom_func func);
nom_func pop_func(stack * stk);
void push_struct(stack * stack, nom_struct nstruct);
nom_struct pop_struct(stack * stk);
//operations
void add(frame * currentframe);
void subtract(frame * currentframe);
void multiply(frame * currentframe);
void divide(frame * currentframe);
void modulus(frame * currentframe);
void negate(frame * currentframe);
//Conditions
void gt(stack * stk);
void gte(stack * stk);
void lt(stack * stk);
void lte(stack * stk);
void eq(stack * stk);
void ne(stack * stk);
//Logical operations
void and(frame * currentframe);
void not(frame * currentframe);
void nand(frame * currentframe);
void or(frame * currentframe);
void nor(frame * currentframe);

//Helper functions
int alphabetical(char * a, char * b);
void copy_struct(frame * currentframeframe, nom_variable * var, nom_struct ns);

static void(*operation[11])(frame * currentframe) = {&add, &subtract, &multiply, &divide, &modulus, &negate, &and, &not, &or, &nor, &nand};
static void(*condition[6])(stack*stk) = {&gt, &gte, &lt, &lte, &eq, &ne};
#endif
