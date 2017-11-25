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

#include "ncomp.h"

//Pushes an instruction into the list given instruction and operator
void push_instr(instr_list * instrl, int instr,  int op)
{
	instrl->num_instructions++;
	if (instrl->num_instructions == 1)
	{
		instrl->instructions = malloc(sizeof(cinstr));
	}
	else
	{
		instrl->instructions = realloc(instrl->instructions, instrl->num_instructions * sizeof(cinstr));
		if (instrl->instructions == NULL) {
			printf("Realloc Failed\n");
		}
	}
	cinstr c;
	c.action = instr;
	c.idx = op;
	instrl->instructions[instrl->num_instructions - 1] = c;
}

//Compiles AST into instructions
void  compile(node * bop, frame * currentframe)
{
	instr_list * instrl = malloc(sizeof(instr_list));
	instrl->num_instructions = 0;
	instrl->instructions = NULL;
	val_traverse(bop, instrl, currentframe);
	//push_instr(instrl, PRINT, 0);
	free_nodes(bop);

	currentframe->instructions = instrl->instructions;
	currentframe->num_instructions = instrl->num_instructions;
	free(instrl);
}


int push_member_idx(node * node, instr_list * instrl, frame * currentframe, nom_variable ** parent)
{
	int args = 0;
	nom_variable * np = NULL;
	if (node->val.type == IDENTIFIER) {
		if (*parent == NULL) {
			int idx = get_var_index(currentframe, node->val.tok);
			if (idx == -1)
			{
				create_var(currentframe, node->val.tok, NONE);
				idx = currentframe->num_variables - 1;
			}
			args++;
			push_instr(instrl, PUSH_IDX, idx);
			*parent = &currentframe->variables[idx];
		} else {
			nom_variable * p = *parent;
			p->type = STRUCT;
			int idx = get_var_index_local(p, node->val.tok);
			if (idx == -1)
			{
				nom_variable * mems = p->members;
				create_var_local(p, node->val.tok, NUM);
				gc_add(currentframe->gcol, p->members[p->num_members-1].name);
				//printf("sad %s.%s\n", lvar->name, cnode->left->right->val.tok);
				if (mems == NULL) {
					gc_add(currentframe->gcol, p->members);
				} else if (mems != p->members) {
					gc_replace(currentframe->gcol, p->members, mems);
				}
				idx = p->num_members - 1;
			}
			args++;
			push_instr(instrl, PUSH_IDX, idx);
			np = &p->members[idx];
		}
	} else {
		if (!node->left || !node->right) syntax_error(node->val.tok, node->val.col, node->val.row, "Unable to find index");
		args += push_member_idx(node->left, instrl, currentframe, parent);
		args += push_member_idx(node->right, instrl, currentframe, parent);
	}
	//a.(b.e)
	if (np != NULL) *parent = np;
	return args;
}
int push_member_str(node * node, instr_list * instrl, frame * currentframe, nom_variable ** parent)
{
	int args = 0;
	if (node->val.type == IDENTIFIER) {
		printf("%s\n", node->val.tok);
		//push_instr(instrl, PUSH_STR, add_const(currentframe, strdup(node->val.tok)));
		args++;
	} else {
		args += push_member_str(node->left, instrl, currentframe, parent);
		args += push_member_str(node->right, instrl, currentframe, parent);
	}
	//a.(b.e)
	return args;
}

//Gets index of variable by searching through the data structure (The eg x.y.z index)
int push_member_idx2(node * node, instr_list * instrl, frame * currentframe)
{
	printf("START\n");
	int args = 0;
	if (node->val.type == IDENTIFIER) {
		int idx = get_var_index(currentframe, node->val.tok);
		if (idx == -1)
		{
			create_var(currentframe, node->val.tok, NONE);
			idx = currentframe->num_variables - 1;
		}
		args++;
		push_instr(instrl, PUSH_IDX, idx);
	}
	else {
		struct node * cnode = node;
		int idx = -1;

		if (cnode->left->val.type == DOT) {
			args += push_member_idx2(cnode->left, instrl, currentframe);
			//printf("DSASDA %s\n",  cnode->left->right->val.tok);
			idx = get_var_index(currentframe, cnode->left->right->val.tok);
			printf("FINAL %s\n", cnode->left->right->val.tok);
			if (idx == -1)
			{
				//nom_variable * mems = lvar->members;
				//create_var_local(lvar, cnode->left->right->val.tok, NUM);
				//printf("sad %s.%s\n", lvar->name, cnode->left->right->val.tok);
				//if (mems == NULL) {
				//	gc_add(currentframe->gcol, lvar->members);
				//} else if (mems != lvar->members) {
				//	gc_replace(currentframe->gcol, lvar->members, mems);
				//}
				//idx = lvar->num_members - 1;
			}
		} else {
			idx = get_var_index(currentframe, cnode->left->val.tok);printf("FINAL %s\n", cnode->left->val.tok);
			if (idx == -1)
			{
				if (cnode->left->val.type == DOT) {
					create_var(currentframe, cnode->left->right->val.tok, NUM);
				}
				else create_var(currentframe, cnode->left->val.tok, NUM);
				idx = currentframe->num_variables - 1;
			}
		}

		args++;

		printf("idx %d\n", idx);
		push_instr(instrl, PUSH_IDX, idx);
		nom_variable * lvar = &currentframe->variables[idx];

		if (cnode->right->val.type == IDENTIFIER) {

			idx = get_var_index_local(lvar, cnode->right->val.tok);

			if (idx == -1)
			{
				nom_variable * mems = lvar->members;
				create_var_local(lvar, cnode->right->val.tok, NUM);
				printf("sad %s.%s\n", lvar->name, cnode->right->val.tok);
				if (mems == NULL) {
					gc_add(currentframe->gcol, lvar->members);
				} else if (mems != lvar->members) {
					gc_replace(currentframe->gcol, lvar->members, mems);
				}
				idx = lvar->num_members - 1;

			}
			args++;

			printf("idx2 %d\n", idx);

			push_instr(instrl, PUSH_IDX, idx);
		}
		else {

			/*
			cnode = cnode->right;
			lvar = &currentframe->variables[idx];
			while (1) {
				idx = get_var_index_local(lvar, cnode->left->val.tok);

				if (idx == -1)
				{
					nom_variable * mems = lvar->members;
					create_var_local(lvar, cnode->left->val.tok, NUM);
					if (mems == NULL) {
						gc_add(currentframe->gcol, lvar->members);
					} else if (mems != lvar->members) {
						gc_replace(currentframe->gcol, lvar->members, mems);
					}
					idx = lvar->num_members - 1;
				}
				args++;
				printf("idx3 %d\n", idx);

				push_instr(instrl, PUSH_IDX, idx);
				lvar = &lvar->members[idx];
				if (cnode->right->val.type == IDENTIFIER) {
					idx = get_var_index_local(lvar, cnode->right->val.tok);
					if (idx == -1)
					{
						nom_variable * mems = lvar->members;
						create_var_local(lvar, cnode->right->val.tok, NUM);
						if (mems == NULL) {
							gc_add(currentframe->gcol, lvar->members);
						} else if (mems != lvar->members) {
							gc_replace(currentframe->gcol, lvar->members, mems);
						}
						idx = lvar->num_members - 1;
					}
					args++;
					printf("idx4 %d\n", idx);

					push_instr(instrl, PUSH_IDX, idx);
					break;
				}
				else if (cnode->right->val.type == DOT) {
					cnode = cnode->right;
				}
			}
			*/
		}
	}
	printf("END\n");

	return args;
}


void traverse(node * node)
{
	if (node == NULL)
		return;
	traverse(node->left);
	traverse(node->right);

	printf("%s ", node->val.tok);
}

//Traverses the tree and creates instructions from each node
void val_traverse(node * node, instr_list * instrl, frame * currentframe)
{
	if (!node)
		return;
	int start_cond = 0;

	if (node->val.type == FUNCTION)
	{
		frame * nf = frame_init();

		nom_func * func = malloc(sizeof(nom_func));
		func->external = 0;
		func->arg_count = node->num_branches - 1;
		func->frame = nf;
		nf->parent = currentframe;
		nf->gcol = currentframe->gcol;
		frame_add_child(currentframe, nf);
		for (int i = 1; i < node->num_branches; i++)
		{
			if (node->branches[i])
			{
				if (node->branches[i]->val.type == IDENTIFIER)
				{
					int idx = get_var_index(nf, node->branches[i]->val.tok);
					if (idx == -1)
					{
						create_var(nf, node->branches[i]->val.tok, NUM);
						idx = nf->num_variables - 1;
					}

				}
				else
					syntax_error(node->val.tok, node->val.col, node->val.row, "Function has invalid parameters");
			}
		}

		compile(node->branches[0], nf);
		node->branches[0] = NULL;

		push_instr(instrl, PUSH_FUNC, add_const(currentframe, func));
		return;
	}
	else if (node->val.type == FUNC_CALL)
	{

		int args = 0;
		for (int i = 1; i < node->num_branches; i++)
		{
			if (node->branches[i]) {
				val_traverse(node->branches[i], instrl, currentframe);				
			}

			args++;
		}
		nom_number * val = malloc(sizeof(nom_number));
		*val = args;
		push_instr(instrl, PUSH, add_const(currentframe, val));
		nom_variable * p = NULL;
		int nargs = push_member_idx(node->branches[0], instrl, currentframe, &p);
		push_instr(instrl, CALL, nargs);
		return;
	}
	else if (node->val.type == INT)
	{
		if (node->val.tok == 0) {
			push_instr(instrl, PUSH, add_const(currentframe, 0));
			return;
		}
		nom_number * val = malloc(sizeof(nom_number));

		*val = atoi(node->val.tok);
		push_instr(instrl, PUSH, add_const(currentframe, val));
		return;
	}
	else if (node->val.type == FLOAT)
	{
		nom_number * val = malloc(sizeof(nom_number));
		*val = atof(node->val.tok);
		push_instr(instrl, PUSH, add_const(currentframe, val));
		return;
	}
	else if (node->val.type == STRING)
	{
		int size = strlen(node->val.tok);
		char * str = malloc(size+1);
		memset(str, 0, size + 1);
		int j = 0;
		for (int i = 0; i < size && j < size; i++)
		{
			char c = node->val.tok[j];
			if (c == '\\')
			{
				j++;
				switch (node->val.tok[j])
				{
				case 'n':
					c = '\n';
					break;
				case 'r':
					c = '\r';
					break;
				case 't':
					c = '\t';
					break;
				default:
					j--;
					break;
				}
								
			}
			str[i] = c;
			j++;
		}
		str[size] = '\0';
		push_instr(instrl, PUSH_STR, add_const(currentframe, str));
	}
	else if (node->val.type == DOT)
	{
		if (node->left->val.type != IDENTIFIER && node->left->val.type != DOT && node->left->val.type == MEM_IDX)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->left->val.tok);
			abort();
		}
		nom_variable * p = NULL;
		//push_member_str(node, instrl, currentframe, &p);
		int args = push_member_idx(node, instrl, currentframe, &p);
		push_instr(instrl, LOAD_NAME, args);
		return;
	}
	else if (node->val.type == IDENTIFIER)
	{
		int idx = get_var_index(currentframe, node->val.tok);

		if (idx == -1)
		{
			create_var(currentframe, node->val.tok, NUM);
			idx = currentframe->num_variables - 1;
		}
		push_instr(instrl, PUSH_IDX, idx);
		push_instr(instrl, LOAD_NAME, 1);
		return;
	}
	else if (node->val.type == EQUAL)
	{
		//STORE_NAME
		//push_instr(instrl, );
		if (node->left->val.type != IDENTIFIER && node->left->val.type != DOT && node->left->val.type != MEM_IDX)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->right->val.tok);
			abort();
		}
		else
		{
			if (node->right)
				val_traverse(node->right, instrl, currentframe);
			nom_variable * p = NULL;
			int args = 0;
			if (node->left->val.type == MEM_IDX) {
				if (node->left->right)
					val_traverse(node->left->right, instrl, currentframe);
				args = push_member_idx(node->left->left, instrl, currentframe, &p);
				push_instr(instrl, ARR_STORE, args);
			} else {
				args = push_member_idx(node->left, instrl, currentframe, &p);
				push_instr(instrl, STORE_NAME, args);
			}
			
		}
		return;
	}
	else if (node->val.type == MEM_IDX)
	{
		if (node->left->val.type != IDENTIFIER && node->left->val.type != DOT)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->left->val.tok);
			abort();
		}
		else
		{
			if (node->right)
				val_traverse(node->right, instrl, currentframe);
			nom_variable * p = NULL;
			int args = push_member_idx(node->left, instrl, currentframe, &p);
			push_instr(instrl, ARR_LOAD, args);
		}	
		return;
	}
	else if (node->val.type == INC)
	{
		//STORE_NAME
		//push_instr(instrl, );

		if (node->left->val.type != IDENTIFIER && node->left->val.type != DOT)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->next->val.tok);
			abort();
		}
		else
		{
			nom_variable * p = NULL;
			int args = push_member_idx(node->next, instrl, currentframe, &p);
			push_instr(instrl, LOAD_NAME, args);
			nom_number * val = malloc(sizeof(nom_number));
			*val = 1;
			push_instr(instrl, PUSH, add_const(currentframe, val));
			push_instr(instrl, ADD, 0);
			p = NULL;
			args = push_member_idx(node->next, instrl, currentframe, &p);
			push_instr(instrl, STORE_NAME, args);		
		}
		return;
	}
	else if (node->val.type == DEC)
	{

		//STORE_NAME
		//push_instr(instrl, );
		if (node->left->val.type != IDENTIFIER && node->left->val.type != DOT)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->next->val.tok);
			abort();
		}
		else
		{
			nom_variable * p = NULL;
			int args = push_member_idx(node->next, instrl, currentframe, &p);
			push_instr(instrl, LOAD_NAME, args);
			nom_number * val = malloc(sizeof(nom_number));
			*val = -1;
			push_instr(instrl, PUSH, add_const(currentframe, val));
			push_instr(instrl, ADD, 0);
			p = NULL;
			args = push_member_idx(node->next, instrl, currentframe, &p);
			push_instr(instrl, STORE_NAME, args);
		}
		return;
	}
	else if (node->val.type == WHILE)
	{
		start_cond = instrl->num_instructions;
		if (node->left)
			val_traverse(node->left, instrl, currentframe);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->right)
			val_traverse(node->right, instrl, currentframe);
		push_instr(instrl, JUMP, start_cond);
		instrl->instructions[idx].idx = instrl->num_instructions;

		return;
	}
	else if (node->val.type == IF)
	{
		start_cond = instrl->num_instructions;
		if (node->left)
			val_traverse(node->left, instrl, currentframe);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->right)
			val_traverse(node->right, instrl, currentframe);
		instrl->instructions[idx].idx = instrl->num_instructions;

		return;
	}
	else if (node->val.type == IFELSE)
	{

		start_cond = instrl->num_instructions;
		if (node->left)
			val_traverse(node->left, instrl, currentframe);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->right)
			val_traverse(node->right, instrl, currentframe);
		instrl->instructions[idx].idx = instrl->num_instructions+1;

		return;
	}
	else if (node->val.type == ELSEIF)
	{
		start_cond = instrl->num_instructions;
		int idx2 = instrl->num_instructions;
		push_instr(instrl, JUMP, start_cond);
		if (node->left)
			val_traverse(node->left, instrl, currentframe);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->right)
			val_traverse(node->right, instrl, currentframe);
		instrl->instructions[idx].idx = instrl->num_instructions;
		instrl->instructions[idx2].idx = instrl->num_instructions;

		return;
	}
	else if (node->val.type == ELSEIFELSE)
	{
		start_cond = instrl->num_instructions;
		int idx2 = instrl->num_instructions;
		push_instr(instrl, JUMP, start_cond);
		if (node->left)
			val_traverse(node->left, instrl, currentframe);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->right)
			val_traverse(node->right, instrl, currentframe);
		instrl->instructions[idx].idx = instrl->num_instructions + 1;
		instrl->instructions[idx2].idx = instrl->num_instructions;
		return;
	}
	else if (node->val.type == ELSE)
	{
		start_cond = instrl->num_instructions;
		int idx = instrl->num_instructions;
		push_instr(instrl, JUMP, start_cond);
		if (node->next)
			val_traverse(node->next, instrl, currentframe);
		instrl->instructions[idx].idx = instrl->num_instructions;

		return;
	}
	else if (node->val.type == FOR)
	{
		if (node->branches[0])
			val_traverse(node->branches[0], instrl, currentframe);
		start_cond = instrl->num_instructions;
		if (node->branches[1])
			val_traverse(node->branches[1], instrl, currentframe);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->branches[3])
			val_traverse(node->branches[3], instrl, currentframe);
		if (node->branches[2])
			val_traverse(node->branches[2], instrl, currentframe);
		push_instr(instrl, JUMP, start_cond);
		instrl->instructions[idx].idx = instrl->num_instructions;

		return;
	}

	if (node->type == BINARY)
	{
		if (node->left)
			val_traverse(node->left, instrl, currentframe);
		if (node->right)
			val_traverse(node->right, instrl, currentframe);
	}
	else if (node->type == UNARY)
	{
		if (node->next)
			val_traverse(node->next, instrl, currentframe);
	}
	else if (node->type == MULTI)
	{
		for (int i = 0; i < node->num_branches; i++)
		{
			if (node->branches[i])
				val_traverse(node->branches[i], instrl, currentframe);
		}
	}

	if (node->val.type == PLUS)
	{
		push_instr(instrl, ADD, 0);
		return;
	}
	else if (node->val.type == MINUS)
	{
		push_instr(instrl, SUB, 0);
		return;
	}
	else if (node->val.type == MULT)
	{
		push_instr(instrl, MUL, 0);
		return;
	}
	else if (node->val.type == DIVIDE)
	{
		push_instr(instrl, DIV, 0);
		return;
	}
	else if (node->val.type == MODULUS)
	{
		push_instr(instrl, MOD, 0);
		return;
	}
	else if (node->val.type == GREATER)
	{
		push_instr(instrl, GT, 0);
		return;
	}
	else if (node->val.type == GREATER_OR_EQ)
	{
		push_instr(instrl, GTE, 0);
		return;
	}
	else if (node->val.type == LESS)
	{
		push_instr(instrl, LT, 0);
		return;
	}
	else if (node->val.type == LESS_OR_EQ)
	{
		push_instr(instrl, LTE, 0);
		return;
	}
	else if (node->val.type == IS_EQUAL)
	{
		push_instr(instrl, EQ, 0);
		return;
	}
	else if (node->val.type == NOT_EQUAL)
	{
		push_instr(instrl, NE, 0);
		return;
	}
	else if (node->val.type == UNARY_NEG)
	{
		push_instr(instrl, NEG, 0);
		return;
	}
	else if (node->val.type == LNOT)
	{
		push_instr(instrl, NOT, 0);
		return;
	}
	else if (node->val.type == LAND)
	{
		push_instr(instrl, AND, 0);
		return;
	}
	else if (node->val.type == LOR)
	{
		push_instr(instrl, OR, 0);
		return;
	}
	else if (node->val.type == LNAND)
	{
		push_instr(instrl, NAND, 0);
		return;
	}
	else if (node->val.type == LNOR)
	{
		push_instr(instrl, NOR, 0);
		return;
	}
	else if (node->val.type == RETURN)
	{
		push_instr(instrl, RET, 0);
		return;
	}
}

