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

#include "cinterp.h"
#include <math.h>
#pragma warning(disable:4996)


#ifdef __linux__
#define STRDUP strdup
#elif _WIN32
#define STRDUP _strdup
#endif

nom_interp * nom_interp_init()
{
	nom_interp * nom = malloc(sizeof(nom_interp));
	nom->global_frame = frame_init();
	nom->global_frame->gcol = gc_init();
	return nom;
}

void nom_interp_destroy(nom_interp * nom)
{
	exit_frame(nom->global_frame);
	free(nom);
}

frame *  frame_init()
{
	frame * cframe = malloc(sizeof(frame));
	cframe->parent = NULL;
	cframe->data_stack = stack_init();
	cframe->instructions = NULL;
	cframe->instr_ptr = 0;
	cframe->num_instructions = 0;
	cframe->variables = NULL;
	cframe->num_variables = 0;
	cframe->constants = NULL;
	cframe->num_constants = 0;
	cframe->children = NULL;
	cframe->num_children = 0;
	cframe->gcol = NULL;

	return cframe;
}

//Copy constants and variables from a frame 
frame * frame_cpy(frame * original)
{
	frame * cframe = frame_init();
	/* TODO */
	return cframe;
}

void frame_add_child(frame * parent, frame * child)
{
	if (parent->num_children == 0) {
		parent->num_children++;
		parent->children = malloc(sizeof(frame*));
	} else {
		parent->num_children++;
		parent->children = realloc(parent->children, sizeof(frame*) * parent->num_children);
		if (parent->children == NULL) {
			printf("Realloc Failed child\n");
		}
	}
	parent->children[parent->num_children-1] = child;
}

void exit_frame(frame * frame)
{
	//Current problem is function copies free constant strings, which are freed again 
	//When function definition frame is exited. 
	while (frame->data_stack->num_elements > 0)
	{
		element e = frame->data_stack->elements[frame->data_stack->num_elements - 1];
		if (e.type == STR)
		{
			nom_string s = pop_string(frame->data_stack);  //TODO SAFELY FREE HERE (Possible fix)
			gc_free(frame->gcol, s.str);
			s.str = NULL;
			
		}
		else pop_element(frame->data_stack);
	}

	stack_destroy(frame->data_stack);

	for (int i = 0; i < frame->num_variables; i++)
	{
		if (frame->variables[i].num_members > 0) {
			nom_var_free_members(frame, &frame->variables[i]);
		} else {
			gc_free(frame->gcol, frame->variables[i].name);
			
			if (frame->variables[i].type == STR) {
				nom_string s = *(nom_string*)frame->variables[i].value;
				gc_free(frame->gcol, s.str);
			} else {
				if (frame->variables[i].value) gc_free(frame->gcol, frame->variables[i].value);
			}
		}
	}
	if (frame->variables) free(frame->variables);
	for (int i = 0; i < frame->num_constants; i++)
	{
		if (frame->constants[i]) free(frame->constants[i]);
		frame->constants[i] = NULL;
	}
	if (frame->constants) free(frame->constants);
	if (frame->instructions) free(frame->instructions);
	for (int i = 0; i < frame->num_children; i++) 
	{
		if (frame->children[i] != NULL) {
			exit_frame(frame->children[i]);
		}
	}
	if (frame->num_children > 0) free(frame->children);
	if (frame->parent) {
		for (int i = 0; i < frame->parent->num_children; i++) 
		{
			if (frame->parent->children[i] == frame) {
				frame->parent->children[i] = NULL;
			}
		}
	}
	if (frame->parent == NULL) {
		gc_destroy(frame->gcol);
	}
	free(frame);
	frame = NULL;
}

void destroy_frame(frame * frame)
{
}
int add_const(frame * frame, void * val)
{
	for (int i = 0; i < frame->num_constants; i++) {
		if (frame->constants[i] == val)
			return i;
	}
	frame->num_constants++;
	if (frame->num_constants == 1)
	{
		frame->constants = malloc(sizeof(void*) * frame->num_constants);

	}
	else
	{
		frame->constants = realloc(frame->constants, sizeof(void*) * frame->num_constants);
		if (frame->constants == NULL) {
			printf("Realloc Failed const\n");
		}
	}
	
	int idx = frame->num_constants - 1;
	frame->constants[idx] = val;

	return idx;
}

void resize_string(nom_string * str, char * nstr, int size)
{
	if (str->num_characters != size)
	{
		if (str->num_characters == 0)
		{
			str->str = malloc(size + 1);
		}
		else {
			str->str = realloc(str->str, size + 1);
			if (str->str == NULL) {
				printf("Realloc Failed str\n");
			}
		}
	}
	for (int i = 0; i < size; i++)
	{
		str->str[i] = nstr[i];
	}
	str->str[size] = '\0';
}

void change_type(frame * frame, nom_variable * old, int ntype)
{
	if (old->type != ntype)
	{
		if (old->type == NONE)
		{
			old->type = NUM;
		}
		else if (old->type != ntype)
		{
			old->type = NUM;
		}
		gc_free(frame->gcol, old->value);
		old->value = malloc(nom_type_size[ntype]);
		gc_add(frame->gcol, old->value);
	}
	old->type = ntype;
}

int get_var_index(frame * currentframe, char * name)
{
	if (currentframe == NULL)
		return -1;

	for (int i = 0; i < currentframe->num_variables; i++)
	{
		if (!strcmp(name, currentframe->variables[i].name))
			return i;
	}
	return -1;
}

void create_var(frame * currentframe, char * name, int type)
{
	nom_variable var;
	var.name = NULL;
	var.name = STRDUP(name);
	var.type = NONE;
	var.value = NULL;
	var.num_references = 1;
	var.num_members = 0;
	var.members = NULL;
	var.member_ref = 1;
	var.parent = NULL;


	currentframe->num_variables++;
	if (currentframe->num_variables == 1)
	{
		currentframe->variables = malloc(sizeof(nom_variable));
	}
	else
	{
		currentframe->variables = realloc(currentframe->variables, sizeof(nom_variable) * currentframe->num_variables);
		if (currentframe->variables == NULL) {
			printf("Realloc Failed var\n");
		}
	}
	if (currentframe->gcol) gc_add(currentframe->gcol, var.name);
	else if (currentframe->parent) {
		currentframe->gcol = currentframe->parent->gcol;
		gc_add(currentframe->gcol, var.name);
	}
	currentframe->variables[currentframe->num_variables - 1] = var;
}

int get_var_index_local(nom_variable * lvar, char * name)
{
	if (lvar == NULL)
		return -1;

	for (int i = 0; i < lvar->num_members; i++)
	{
		if (!strcmp(name, lvar->members[i].name))
			return i;
	}
	return -1;
}

void create_var_local(nom_variable * lvar, char * name, int type)
{
	nom_variable var;
	var.name = STRDUP(name);
	var.type = NONE;
	var.value = NULL;
	var.num_references = 1;
	var.num_members = 0;
	var.members = NULL;
	var.member_ref = 1;
	var.parent = lvar;

	lvar->num_members++;

	if (lvar->num_members == 1)
	{
		lvar->members = malloc(sizeof(nom_variable));
	}
	else
	{
		lvar->members = realloc(lvar->members, sizeof(nom_variable) * lvar->num_members);
		if (lvar->members == NULL) {
			printf("Realloc Failed mem\n");
		}
	}
	lvar->members[lvar->num_members - 1] = var;
	lvar->type = STRUCT;
}

void nom_var_free_members(frame * frame, nom_variable * var)
{
	//TODO make this work
	//Todo fix this. Currently causes crashes and doesnt free everything
	//return;
	if (!var->members || var->num_members <= 0 || !var)
		return;
	for (int j = 0; j < var->num_members; j++) {
		nom_variable * nvar = &var->members[j];

		if (nvar->value && var->type != STRUCT) gc_free(frame->gcol, nvar->value);
		if (var->type == STRUCT) {
			if (nvar->num_members > 0)
				nom_var_free_members(frame, nvar);
		}
		gc_free(frame->gcol, nvar->name);
	}
	gc_free(frame->gcol, var->members);
	gc_free(frame->gcol, var->value);
	gc_free(frame->gcol, var->name);
}

void nom_var_add_ref(frame * frame, nom_variable * var)
{
	for (int j = 0; j < var->num_members; j++) {
		nom_variable * nvar = &var->members[j];
		
		if (nvar->num_members > 0) {
			gc_add(frame->gcol, nvar->members);
			nom_var_add_ref(frame, nvar);
		} else if (nvar->value) {
			gc_add(frame->gcol, nvar->value);
		}
		gc_add(frame->gcol, nvar->name);
	}
	if (var->num_members > 0) gc_add(frame->gcol, var->members);
	if (var->value) gc_add(frame->gcol, var->value);
	gc_add(frame->gcol, var->name);
}

void copy_struct(frame * currentframe, nom_variable * var, nom_struct ns)
{

	//Copys a struct into a variable. Accounts for different idxs and recursively copies for members that are structs
	int * vi = malloc(sizeof(int) * var->num_members);
	int * ni = malloc(sizeof(int) * ns.num_members);
	for(int i = 0; i < var->num_members; i++) {
		vi[i] = -1;
	}
	for(int i = 0; i < ns.num_members; i++) {
		ni[i] = 0;
	}
	int oldlen = var->num_members;
	for (int i = 0; i < ns.num_members; i++) {
		int idx = get_var_index_local(var, ns.members[i].name);
		if (idx != -1) {
			vi[idx] = i;
		} else{
			ni[i] = 1;
		}
	}
	//if (var->num_members > 0) nom_var_free_members(currentframe, var);

	nom_variable * oldmembers = var->members;

	var->num_members = 0;
	for (int i = 0; i < (ns.num_members > oldlen ? ns.num_members : oldlen); i++) {
		if (i < oldlen) {
			if (vi[i] != -1) {
				create_var_local(var, ns.members[vi[i]].name, ns.members[vi[i]].type);
				free(var->members[var->num_members-1].name);
				var->members[var->num_members-1].name = ns.members[vi[i]].name;
				var->members[var->num_members-1].type = ns.members[vi[i]].type;
				var->members[var->num_members-1].value = ns.members[vi[i]].value;
				if (ns.members[vi[i]].num_members > 0) {
					nom_struct mns;
					mns.members = ns.members[vi[i]].members;
					mns.num_members = ns.members[vi[i]].num_members;
					copy_struct(currentframe, &var->members[var->num_members-1], mns);
				}
				gc_add(currentframe->gcol, var->members[var->num_members-1].value);
			} else { 
				create_var_local(var, oldmembers[i].name, oldmembers[i].type);
				free(var->members[var->num_members-1].name);
				var->members[var->num_members-1].name = oldmembers[i].name;
				var->members[var->num_members-1].type = oldmembers[i].type;
				var->members[var->num_members-1].value = oldmembers[i].value;
				if (oldmembers[i].num_members > 0) {
					nom_struct mns;
					mns.members = oldmembers[i].members;
					mns.num_members = oldmembers[i].num_members;
					copy_struct(currentframe, &var->members[var->num_members-1], mns);
				}
				gc_add(currentframe->gcol, var->members[var->num_members-1].value);
			}
		} else {
			create_var_local(var, ns.members[i].name, ns.members[i].type);
			free(var->members[var->num_members-1].name);
			var->members[var->num_members-1].name = ns.members[i].name;
			var->members[var->num_members-1].type = ns.members[i].type;
			var->members[var->num_members-1].value = ns.members[i].value;
			if (ns.members[i].num_members > 0) {
				nom_struct mns;
				mns.members = ns.members[i].members;
				mns.num_members = ns.members[i].num_members;
				copy_struct(currentframe, &var->members[var->num_members-1], mns);
			}
			gc_add(currentframe->gcol, var->members[var->num_members-1].value);
		}
	}
	//The newly added vars go on the end
	for (int i = 0; i < (ns.num_members > oldlen ? ns.num_members : oldlen); i++) {
		if (i < ns.num_members && ni[i]) {
			create_var_local(var, ns.members[i].name, ns.members[i].type);
			free(var->members[var->num_members-1].name);
			var->members[var->num_members-1].name = ns.members[i].name;
			var->members[var->num_members-1].type = ns.members[i].type;
			var->members[var->num_members-1].value = ns.members[i].value;
			if (ns.members[i].num_members > 0) {
				nom_struct mns;
				mns.members = ns.members[i].members;
				mns.num_members = ns.members[i].num_members;
				copy_struct(currentframe, &var->members[var->num_members-1], mns);
			}
			gc_add(currentframe->gcol, var->members[var->num_members-1].value);
		}
	}

	//gc_remove(currentframe->gcol, oldmembers);
	//gc_free(currentframe->gcol, oldmembers);
	//var->members = ns.members;
	gc_add(currentframe->gcol, var->members);
	nom_var_add_ref(currentframe, var); 
	var->member_ref = ns.mem_ref;
	//gc_free(currentframe->gcol, ns.members);
	free(vi);
	free(ni);	
}

void execute(frame * currentframe)
{
	if (currentframe->parent) currentframe->gcol = currentframe->parent->gcol;

	currentframe->instr_ptr = 0;
	while (currentframe->instr_ptr < currentframe->num_instructions)
	{

		cinstr c = currentframe->instructions[currentframe->instr_ptr];
		currentframe->instr_ptr++;
		if (c.action < 11) {
			operation[c.action](currentframe);
		}
		else if (c.action < 17) {
			condition[c.action-11](currentframe->data_stack);
		}
		else if (c.action < 19)
		{
			if (c.action == IFEQ)
			{
				nom_number n;
				pop_store(currentframe->data_stack, sizeof(nom_number), &n);

				if (n == 0)
					currentframe->instr_ptr = c.idx;
			}
			else if (c.action == JUMP)
			{
				currentframe->instr_ptr = c.idx;
			}
		}
		else if (c.action == PUSH_IDX)
		{
			push_number(currentframe->data_stack, (nom_number)c.idx);
		}
		else if (c.action == PUSH)
		{
			nom_number * num = currentframe->constants[c.idx];
			push_number(currentframe->data_stack, *num);
		}
		else if (c.action == POP)
		{
			pop(currentframe->data_stack, sizeof(nom_number));
		}
		else if (c.action == PUSH_STR)
		{
			char * str = STRDUP(currentframe->constants[c.idx]);
			//printf("DIS %s\n", str);
			gc_add(currentframe->gcol, str);

			push_raw_string(currentframe->data_stack, str);
		}
		else if (c.action == POP_STR)
		{
			pop(currentframe->data_stack, sizeof(nom_string));
		}
		else if (c.action == PUSH_FUNC)
		{
			nom_func * func = currentframe->constants[c.idx];
			push_func(currentframe->data_stack, *func);
		}
		else if (c.action == POP_FUNC)
		{
			pop(currentframe->data_stack, sizeof(nom_func));
		}
		else if (c.action == LOAD)
		{
			nom_number n;
			store(currentframe->data_stack, &n, sizeof(nom_number), 0);
			load(currentframe->data_stack, &n, sizeof(nom_number), c.idx);
		}
		else if (c.action == DUP)
		{
			dup(currentframe->data_stack);
		}
		else if (c.action == SWAP)
		{
			swap(currentframe->data_stack);
		}
		else if (c.action == PRINT)
		{
			if (currentframe->data_stack->stack_ptr > 0)
			{
				element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
				if (e.type == NUM) {
					nom_number n;
					store(currentframe->data_stack, &n, sizeof(nom_number), 0);
					if (floorf(n) == n)
						printf("%d", (int)n);
					else
						printf("%f", n);
				}
				else if (e.type == BOOL) {
					nom_boolean n;
					store(currentframe->data_stack, &n, sizeof(nom_boolean), 0);
					if (n)
						printf("True");
					else
						printf("False");
				}
				else if (e.type == STR)
				{
					nom_string str;
					store(currentframe->data_stack, &str, sizeof(nom_string), 0);
					printf("%s", str.str);
				}
			}
		}
		else if (c.action == LOAD_NAME)
		{
			int * idxs = malloc(sizeof(int) * c.idx);
			for (int i = 0; i < c.idx; i++) {
				idxs[i] = (int)pop_number(currentframe->data_stack);
			}
			nom_variable * var = &currentframe->variables[idxs[c.idx - 1]];
			for (int i = c.idx - 2; i >= 0; i--) {
				int vidx = idxs[i];
				var = &var->members[vidx];
			}

			free(idxs);
			if (var->type == NUM) {
				push_number(currentframe->data_stack, *((nom_number*)var->value));
			}
			else if (var->type == BOOL) {
				push_bool(currentframe->data_stack, *((nom_boolean*)var->value));
			}
			else if (var->type == STR)
			{
				push_string(currentframe->data_stack, *((nom_string*)var->value));
				gc_add(currentframe->gcol, ((nom_string*)var->value)->str);
			}
			else if (var->type == FUNC)
			{
				push_func(currentframe->data_stack, *((nom_func*)var->value));
				//push_number(currentframe->data_stack, 0);
			}
			if (var->num_members > 0) {
				nom_struct ns;
				ns.num_members = var->num_members;
				ns.members = var->members;
				//gc_add(currentframe->gcol, var->members);
				ns.mem_ref = var->member_ref + 1;
				var->member_ref++;
				push_struct(currentframe->data_stack, ns);
				gc_add(currentframe->gcol, var->members);
				nom_var_add_ref(currentframe, var);
			}
		}
		else if (c.action == STORE_NAME)
		{
			int * idxs = malloc(sizeof(int) * c.idx);
			for (int i = 0; i < c.idx; i++) {
				idxs[i] = (int)pop_number(currentframe->data_stack);
			}
			nom_variable * var = &currentframe->variables[idxs[c.idx-1]];
			for (int i = c.idx-2; i >= 0; i--) {
				int vidx = idxs[i];
				var = &var->members[vidx];
			}

			free(idxs);
			element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
			if (e.type != STRUCT) {
				change_type(currentframe, var, e.type);
			}

			if (e.type == STRUCT) {
				/*
				nom_struct ns = pop_struct(currentframe->data_stack);
				if (var->num_members > 0) nom_var_free_members(currentframe, var);
				var->members = ns.members;
				gc_add(currentframe->gcol, var->members);
				var->num_members = ns.num_members;
				var->member_ref = ns.mem_ref;
				*/
				nom_struct ns = pop_struct(currentframe->data_stack);
				copy_struct(currentframe, var, ns);
			}
			else if (var->type == NUM) {
				*((nom_number*)var->value) = pop_number(currentframe->data_stack);
			}
			else if (var->type == BOOL) {
				*((nom_boolean*)var->value) = pop_bool(currentframe->data_stack);
			}
			else if (var->type == STR)
			{
				*((nom_string*)var->value) = pop_string(currentframe->data_stack);
				gc_add(currentframe->gcol, var->value);
			}
			else if (var->type == FUNC)
			{
				*((nom_func*)var->value) = pop_func(currentframe->data_stack);
			}
		}
		else if (c.action == ARR_LOAD)
		{
			int * idxs = malloc(sizeof(int) * c.idx);
			for (int i = 0; i < c.idx; i++) {
				idxs[i] = (int)pop_number(currentframe->data_stack);
			}
			nom_variable * var = &currentframe->variables[idxs[c.idx - 1]];
			for (int i = c.idx - 2; i >= 0; i--) {
				int vidx = idxs[i];
				var = &var->members[vidx];
			}

			free(idxs);
			int idx = (int)pop_number(currentframe->data_stack);
			var = &var->members[idx];
			if (var->type == NUM) {
				push_number(currentframe->data_stack, *((nom_number*)var->value));
			}
			else if (var->type == BOOL) {
				push_bool(currentframe->data_stack, *((nom_boolean*)var->value));
			}
			else if (var->type == STR)
			{
				push_string(currentframe->data_stack, *((nom_string*)var->value));
				gc_add(currentframe->gcol, ((nom_string*)var->value)->str);
			}
			else if (var->type == FUNC)
			{
				push_func(currentframe->data_stack, *((nom_func*)var->value));
				//push_number(currentframe->data_stack, 0);
			}
			if (var->num_members > 0) {
				nom_struct ns;
				ns.num_members = var->num_members;
				ns.members = var->members;
				//gc_add(currentframe->gcol, var->members);
				ns.mem_ref = var->member_ref + 1;
				var->member_ref++;
				push_struct(currentframe->data_stack, ns);
				gc_add(currentframe->gcol, var->members);
				nom_var_add_ref(currentframe, var);
			}
		}
		else if (c.action == ARR_STORE)
		{
			int * idxs = malloc(sizeof(int) * c.idx);
			for (int i = 0; i < c.idx; i++) {
				idxs[i] = (int)pop_number(currentframe->data_stack);
			}
			nom_variable * var = &currentframe->variables[idxs[c.idx-1]];
			for (int i = c.idx-2; i >= 0; i--) {
				int vidx = idxs[i];
				var = &var->members[vidx];
			}
			free(idxs);
			int idx = (int)pop_number(currentframe->data_stack);
			if (var->num_members <= idx) {
				char buf[16];
				if (var->num_members == idx) {
					snprintf(buf, 16, "%d", idx);
					nom_variable * mems = var->members;
					create_var_local(var, buf, NONE);
					gc_add(currentframe->gcol, var->members[var->num_members-1].name);
					if (mems == NULL) {
						gc_add(currentframe->gcol, var->members);
					} else if (mems != var->members) {
						gc_replace(currentframe->gcol, var->members, mems);
					}
				} else {
					printf("FATAL ERROR: IDX OUT OF BOUNDS IN ARR %s\n", var->name);
					abort();
				}
			}
			var = &var->members[idx];

			element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
			if (e.type != STRUCT) {
				change_type(currentframe, var, e.type);
			}

			if (e.type == STRUCT) {
				/*
				nom_struct ns = pop_struct(currentframe->data_stack);
				if (var->num_members > 0) nom_var_free_members(currentframe, var);
				var->members = ns.members;
				gc_add(currentframe->gcol, var->members);
				var->num_members = ns.num_members;
				var->member_ref = ns.mem_ref;
				*/
				nom_struct ns = pop_struct(currentframe->data_stack);
				copy_struct(currentframe, var, ns);
			}
			else if (var->type == NUM) {
				*((nom_number*)var->value) = pop_number(currentframe->data_stack);
			}
			else if (var->type == BOOL) {
				*((nom_boolean*)var->value) = pop_bool(currentframe->data_stack);
			}
			else if (var->type == STR)
			{
				*((nom_string*)var->value) = pop_string(currentframe->data_stack);
				gc_add(currentframe->gcol, var->value);
			}
			else if (var->type == FUNC)
			{
				*((nom_func*)var->value) = pop_func(currentframe->data_stack);
			}
		}
		else if (c.action == CALL)
		{
			int * idxs = malloc(sizeof(int) * c.idx);
			for (int i = 0; i < c.idx; i++) {
				idxs[i] = (int)pop_number(currentframe->data_stack);
			}
			nom_variable * var = &currentframe->variables[idxs[c.idx - 1]];
			for (int i = c.idx - 2; i >= 0; i--) {
				int vidx = idxs[i];
				var = &var->members[vidx];
			}


			free(idxs);
			nom_func f = *((nom_func*)var->value);

			if (f.external)
			{
				f.func(currentframe);
			}
			else {
				//Keep copy of frame
				frame * oldf = f.frame;
				//Make copy of mutable data and point to old immutable data for function
				// (So make new variables and stack)
				frame * newf = frame_init();
				//For recursive functions, this just alters the caller of the first times stack, not any of the other
				/* TODO */
				//frame_add_child(currentframe, newf); Dont add as child in temp recursive copy
				newf->parent = currentframe;
				newf->constants = oldf->constants;
				newf->num_constants = oldf->num_constants;
				newf->num_instructions = oldf->num_instructions;
				newf->instructions = oldf->instructions;
				int args = pop_number(currentframe->data_stack);
				
				for (int i = 0; i < oldf->num_variables; i++)
				{
					nom_variable v = oldf->variables[i];
					create_var(newf, v.name, v.type);
					//if (i < args)
					//	continue;
					newf->variables[newf->num_variables - 1].num_members = v.num_members;
					newf->variables[newf->num_variables - 1].member_ref = 1;
					//LEAK
					
					if (v.num_members > 0) {
						newf->variables[newf->num_variables - 1].members = malloc(sizeof(nom_variable) * v.num_members);
						//newf->variables[newf->num_variables - 1].members = v.members;
						gc_add(currentframe->gcol, newf->variables[newf->num_variables - 1].members);
					}
					//gc_add(currentframe->gcol, newf->variables[newf->num_variables - 1].members);

					for (int j = 0; j < v.num_members; j++) {
						newf->variables[newf->num_variables-1].members[j] = v.members[j];
					}
				
				}

				f.frame = newf;

				//Set top of stack to function args
				for (int i = 0; i < args; i++)
				{
					nom_variable * v = &f.frame->variables[(args - 1) - i];
					element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
					change_type(currentframe, v, e.type);

					if (v->type == NUM) {
						*((nom_number*)v->value) = pop_number(currentframe->data_stack);

					}
					else if (v->type == BOOL) {
						*((nom_boolean*)v->value) = pop_bool(currentframe->data_stack);
					}
					else if (v->type == STR)
					{
						*((nom_string*)v->value) = pop_string(currentframe->data_stack);
						gc_add(currentframe->gcol, var->value);
					}
					else if (v->type == FUNC)
					{
						*((nom_func*)v->value) = pop_func(currentframe->data_stack);
						gc_add(currentframe->gcol, v->value);
					}
					else if (v->type == STRUCT) {
						nom_struct ns = pop_struct(currentframe->data_stack);
						//Variabled are indexed different on copies, so set them based on name
						//for (int i = 0; i < ns.num_members; i++) {
						//	for (int j = 0; j < v->num_members; j++) {
						//		if (strcmp(v->members[j].name, ns.members[i].name) == 0)
						//			v->members[j] = ns.members[i];
						//	}
						//}
						copy_struct(currentframe, v, ns);
						nom_var_add_ref(currentframe, v);
						//v->member_ref = ns.mem_ref + 1;
						//v->num_members = ns.num_members;

						//gc_add(currentframe->gcol, ns.members);
					}
				}
				//Set variables in function to ones out of its scope
				for (int i = args; i < f.frame->num_variables; i++)
				{
					nom_variable * v = &f.frame->variables[i];
					nom_variable * rv = NULL;
					frame * fp = currentframe;

					while (fp)
					{
						int idx = get_var_index(fp, v->name);
						if (idx != -1)
							rv = &fp->variables[idx];
						fp = fp->parent;
					}
					if (rv && rv->type != STRUCT)
					{
						change_type(currentframe, v, rv->type);
						v->num_references++;
						//free(v->value);
						//gc_free(currentframe, v->value);
						v->value = rv->value;
						gc_add(currentframe->gcol, v->value);
						v->members = rv->members;
						//gc_add(currentframe->gcol, v->members);
						v->num_members = rv->num_members;
						v->member_ref = rv->member_ref;

					}
				}
				//Set self variable
				if (var->parent) {
					int sidx = get_var_index(f.frame, "self");
					if (sidx != -1) {
						nom_variable * v = &f.frame->variables[sidx];
						nom_struct ns;
						ns.members = var->parent->members;
						ns.num_members = var->parent->num_members;
						copy_struct(f.frame, v, ns);
						nom_var_add_ref(currentframe, v);
					}
				}
				execute(f.frame); 
				f.frame->num_constants = 0;
				f.frame->num_instructions = 0;
				f.frame->constants = NULL;
				f.frame->instructions = NULL;
				exit_frame(f.frame);
				f.frame = oldf;
				//push_func(currentframe->data_stack, *((nom_func*)var->value));
			}
		}
		else if (c.action == RET)
		{			
			if (currentframe->data_stack->stack_ptr > 0 && currentframe->parent)
			{
				element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
				if (e.type == NUM) {
					nom_number n;
					store(currentframe->data_stack, &n, sizeof(nom_number), 0);

					push_number(currentframe->parent->data_stack, n);
				}
				else if (e.type == BOOL) {
					nom_boolean n;
					store(currentframe->data_stack, &n, sizeof(nom_boolean), 0);
					push_bool(currentframe->parent->data_stack, n);

				}
				else if (e.type == STR)
				{
					nom_string str;
					store(currentframe->data_stack, &str, sizeof(nom_string), 0);
					char * newstr = STRDUP(str.str);
					gc_add(currentframe->gcol, newstr);
					push_raw_string(currentframe->parent->data_stack, newstr);
					//push_string(currentframe->parent->data_stack, str);
				}
				else if (e.type == STRUCT)
				{
					nom_struct nstruct;
					store(currentframe->data_stack, &nstruct, sizeof(nom_struct), 0);
					gc_add(currentframe->gcol, nstruct.members);
					push_struct(currentframe->parent->data_stack, nstruct);
				} 
			}
			return;
		}
	}

}

stack * stack_init()
{
	stack * stk = malloc(sizeof(stack));
	stk->base_ptr = 0;
	stk->stack_ptr = 0;
	stk->buff = malloc(STACK_CHUNK);
	stk->chunks = 1;
	for (int i = 0; i < STACK_CHUNK; i++)
	{
		stk->buff[i] = 0;
	}
	stk->num_elements = 0;
	stk->elements = NULL;
	return stk;
}

void stack_resize(stack * stk, int size_bytes)
{
	if ((stk->stack_ptr + size_bytes) > (stk->chunks * STACK_CHUNK)) {
		int nchunks = 1;
		if (size_bytes > STACK_CHUNK) {
			int half = STACK_CHUNK/2;
			int rounded_size = size_bytes + half - ((size_bytes+half) % STACK_CHUNK);
			nchunks = (rounded_size/STACK_CHUNK)*2;			
		}
		stk->buff = realloc(stk->buff, nchunks*STACK_CHUNK + stk->chunks*STACK_CHUNK);
		stk->chunks += nchunks;
		for (int i = stk->stack_ptr; i < (stk->chunks*STACK_CHUNK); i++) {
			stk->buff[i] = 0;
		}
	} 
}

void stack_destroy(stack * stack)
{
	if (stack->elements) free(stack->elements);
	free(stack->buff);
	free(stack);
}

void push_element(stack * stk, char * data, int size, int type)
{
	stk->num_elements++;
	if (stk->num_elements == 1)
	{
		stk->elements = malloc(sizeof(element));
	}
	else
	{
		stk->elements = realloc(stk->elements, sizeof(element) * stk->num_elements);
		if (stk->elements == NULL) {
			printf("Realloc Failed push_ele\n");
		}
	}
	element e;
	e.data = data;
	e.size = size;
	e.type = type;
	stk->elements[stk->num_elements - 1] = e;
}

void pop_element(stack * stk)
{
	if (stk->num_elements > 1)
	{
		stk->num_elements--;
		stk->elements = realloc(stk->elements, sizeof(element) * stk->num_elements);
		if (stk->elements == NULL) {
			printf("Realloc Failed pop ele\n");
		}
	} else if (stk->num_elements == 1) {
		stk->num_elements = 0;
		free(stk->elements);
		stk->elements = NULL;
	}
}

void push(stack * stk, void * val, int size_bytes)
{
	char * cval = val;
	stack_resize(stk, size_bytes);
	for (int i = 0; i < size_bytes; i++)
	{
		stk->buff[stk->stack_ptr] = cval[i];
		stk->stack_ptr++;
	}
}

void pop(stack * stk, int size_bytes)
{
	if (size_bytes > (stk->stack_ptr - stk->base_ptr))
		return;
	
	stk->stack_ptr -= size_bytes;
}

void pop_store(stack * stk, int size_bytes, void * buf)
{
	if (size_bytes > (stk->stack_ptr - stk->base_ptr))
		return;
	char * cbuf = buf;
	stack_resize(stk, size_bytes);
	for (int i = 0; i < size_bytes; i++)
	{
		cbuf[i] = stk->buff[stk->stack_ptr-size_bytes + i];
	}
	stk->stack_ptr -= size_bytes;
	pop_element(stk);
}

void store(stack * stk, void * buf, int size_bytes, int offset)
{
	if (size_bytes > (stk->stack_ptr - stk->base_ptr))
		return;
	char * cbuf = buf;
	stack_resize(stk, size_bytes);
	for (int i = 0; i < size_bytes; i++)
	{
		cbuf[i] = stk->buff[(stk->stack_ptr-offset) - size_bytes + i];
	}
}

void load(stack * stk, void * val, int size_bytes, int offset)
{
	char * cval = val;
	stack_resize(stk, size_bytes);
	for (int i = 0; i < size_bytes; i++)
	{
		stk->buff[stk->stack_ptr-offset] = cval[i];
		stk->stack_ptr++;
	}
}

void dup(stack * stk)
{
	if (stk->num_elements < 1)
		return;
	element e = stk->elements[stk->num_elements - 1];
	char * val = malloc(e.size);
	store(stk, val, e.size, 0);
	push(stk, val, e.size);
	push_element(stk, &stk->buff[stk->stack_ptr - e.size], e.size, e.type);
	free(val);
}

void swap(stack * stk)
{
	if (stk->num_elements < 2)
		return;
	element ea = stk->elements[stk->num_elements - 1], eb = stk->elements[stk->num_elements - 2];
	int sizea = ea.size, sizeb = eb.size;
	char * a = malloc(sizea);
	pop_store(stk, sizea, a);
	char * b = malloc(sizeb);
	pop_store(stk, sizeb, b);
	push(stk, a, sizea);
	push(stk, b, sizea);
	stk->elements[stk->num_elements - 1] = eb;
	stk->elements[stk->num_elements - 2] = ea;
}

void push_number(stack * stk, nom_number number)
{
	nom_number n = number;
	push(stk, &n, sizeof(nom_number));
	push_element(stk, &stk->buff[stk->stack_ptr - sizeof(nom_number)], sizeof(nom_number), NUM);
}

nom_number pop_number(stack * stk)
{
	nom_number n;
	pop_store(stk, sizeof(nom_number), &n);
	return n;
}

void push_bool(stack * stk, nom_boolean boolean)
{
	nom_boolean n = boolean;
	push(stk, &n, sizeof(nom_boolean));

	push_element(stk, &stk->buff[stk->stack_ptr - sizeof(nom_boolean)], sizeof(nom_boolean), BOOL);
}

nom_boolean pop_bool(stack * stk)
{
	nom_boolean n;
	pop_store(stk, sizeof(nom_boolean), &n);
	return n;
}

void push_string(stack * stk, nom_string str)
{
	push(stk, &str, sizeof(nom_string));
	push_element(stk, &stk->buff[stk->stack_ptr - sizeof(nom_string)], sizeof(nom_string), STR);
}

void push_raw_string(stack * stack, char * string)
{
	nom_string str;
	str.num_characters = 0;
	if (string) {
		str.str = string;
		str.num_characters = strlen(str.str);
	}
	else str.str = NULL;
	
	//resize_string(&str, string, strlen(string));
	push(stack, &str, sizeof(nom_string));
	push_element(stack, &stack->buff[stack->stack_ptr - sizeof(nom_string)], sizeof(nom_string), STR);
}

nom_string pop_string(stack * stk)
{
	nom_string str;
	pop_store(stk, sizeof(nom_string), &str);
	return str;
}

void push_func(stack * stack, nom_func func)
{
	push(stack, &func, sizeof(nom_func));
	push_element(stack, &stack->buff[stack->stack_ptr - sizeof(nom_func)], sizeof(nom_func), FUNC);
}

nom_func pop_func(stack * stk)
{
	nom_func func;
	pop_store(stk, sizeof(nom_func), &func);
	return func;
}

void push_struct(stack * stack, nom_struct nstruct)
{
	push(stack, &nstruct, sizeof(nom_struct));
	push_element(stack, &stack->buff[stack->stack_ptr - sizeof(nom_struct)], sizeof(nom_struct), STRUCT);
}

nom_struct pop_struct(stack * stk)
{
	nom_struct nstruct;
	pop_store(stk, sizeof(nom_struct), &nstruct);
	return nstruct;
}

void add(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	//Adding numbers or apphending strings
	if (stk->num_elements < 2)
		return;
	int t1, t2;
	t1 = stk->elements[stk->num_elements - 1].type;
	t2 = stk->elements[stk->num_elements - 2].type;
	if (t1 == NUM && t2 == NUM) {
		nom_number a = pop_number(stk), b = pop_number(stk);
		push_number(stk, b + a);
	}
	else if (t1 == STR && t2 == STR) {
		nom_string b = pop_string(stk);
		nom_string a = pop_string(stk);
		int size = a.num_characters + b.num_characters;

		char * str = malloc(size+1);
		memset(str, 0, size);
		for (int i = 0; i < size; i++) {
			if (i < a.num_characters) {
				str[i] = a.str[i];//
			}
			else {
				str[i] = b.str[i - a.num_characters];
			}
		}
		str[size] = '\0';
		gc_add(currentframe->gcol, str);
		push_raw_string(stk, str);
	}
	else if (t1 == STR && t2 == NUM) {
		nom_string a = pop_string(stk);
		nom_number b = pop_number(stk);
		char buf[256];
		memset(buf, 0, 255);
		if (floorf(b) == b)
			sprintf(buf, "%d", (int)b);
		else
			sprintf(buf, "%f", b);
		int bf = strlen(buf);
		int size = a.num_characters + bf;

		char * str = malloc(size+1);
		memset(str, 0, size);
		for (int i = 0; i < size; i++) {
			if (i >= bf) {
				str[i] = a.str[i - bf];
			}
			else {
				str[i] = buf[i];
				
			}
		}
		str[size] = '\0';
		gc_add(currentframe->gcol, str);
		push_raw_string(stk, str);
	}
	else if (t1 == NUM && t2 == STR) {
		nom_number b = pop_number(stk);
		nom_string a = pop_string(stk);

		char buf[256];
		memset(buf, 0, 255);
		if (floorf(b) == b)
			sprintf(buf, "%d", (int)b);
		else
			sprintf(buf, "%f", b);
		int bf = strlen(buf);
		int size = a.num_characters + bf;
		char * str = malloc(size+1);
		memset(str, 0, size);
		for (int i = 0; i < size; i++) {
			if (i < a.num_characters) {
				str[i] = a.str[i];
			}
			else {
				str[i] = buf[i - a.num_characters];
			}
		}
		str[size] = '\0';
		gc_add(currentframe->gcol, str);
		push_raw_string(stk, str);
	}
}

void subtract(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b - a);
}

void multiply(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b * a);
}

void divide(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b / a);
}

void modulus(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, (int)b % (int)a);
}

void negate(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk);
	push_number(stk, -a);
}

void gt(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b > a);
}

void gte(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b >= a);
}

void lt(stack * stk)
{
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b < a);
}

void lte(stack * stk)
{

	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b <= a);

}

void eq(stack * stk)
{
	//Adding numbers or apphending strings
	if (stk->num_elements < 2)
		return;
	int t1, t2;
	t1 = stk->elements[stk->num_elements - 1].type;
	t2 = stk->elements[stk->num_elements - 2].type;
	if (t1 == NUM && t2 == NUM) {
		nom_number a = pop_number(stk), b = pop_number(stk);
		push_number(stk, fabs(b-a) < EPSILON);
	}
	else if (t1 == STR && t2 == STR) {
		nom_string b = pop_string(stk);
		nom_string a = pop_string(stk);
		push_number(stk, !strcmp(a.str, b.str));
	}
	else if (t1 == NUM && t2 == STR) {
		nom_number b = pop_number(stk);
		nom_string a = pop_string(stk);
		push_number(stk, 0);
	}
	else if (t1 == STR && t2 == NUM) {
		nom_string b = pop_string(stk);
		nom_number a = pop_number(stk);
		push_number(stk, 0);
	}
}

void ne(stack * stk)
{
	//Adding numbers or apphending strings
	if (stk->num_elements < 2)
		return;
	int t1, t2;
	t1 = stk->elements[stk->num_elements - 1].type;
	t2 = stk->elements[stk->num_elements - 2].type;
	if (t1 == NUM && t2 == NUM) {
		nom_number a = pop_number(stk), b = pop_number(stk);
		push_number(stk, b != a);
	}
	else if (t1 == STR && t2 == STR) {
		nom_string b = pop_string(stk);
		nom_string a = pop_string(stk);
		push_number(stk, strcmp(a.str, b.str));
	}
}

void and(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b && a);
}

void not(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk);
	push_number(stk, !a);
}

void nand(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, !(b && a));
}

void or(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, b || a);
}

void nor(frame * currentframe)
{
	stack * stk = currentframe->data_stack;
	nom_number a = pop_number(stk), b = pop_number(stk);
	push_number(stk, !(b || a));
}