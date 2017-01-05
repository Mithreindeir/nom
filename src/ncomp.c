#include "ncomp.h"

void push_instr(instr_list * instrl, int instr, float op)
{
	instrl->num_instructions++;
	if (instrl->num_instructions == 1)
	{
		instrl->instructions = malloc(sizeof(cinstr));
	}
	else
	{
		instrl->instructions = realloc(instrl->instructions, instrl->num_instructions * sizeof(cinstr));
	}
	cinstr c;
	c.action = instr;
	c.idx = op;
	instrl->instructions[instrl->num_instructions - 1] = c;
}

void  compile(node * bop, frame * currentframe)
{
	instr_list * instrl = malloc(sizeof(instr_list));
	instrl->num_instructions = 0;
	instrl->instructions = NULL;
	val_traverse(bop, instrl, currentframe);
	push_instr(instrl, PRINT, 0);
	free_nodes(bop);

	currentframe->instructions = instrl->instructions;
	currentframe->num_instructions = instrl->num_instructions;
	free(instrl);
}


void traverse(node * node)
{
	if (node == NULL)
		return;
	traverse(node->left);
	traverse(node->right);

	printf("%s ", node->val.tok);
}
void tree_traverse(node * node)
{
	printf(" %s ", node->val.tok);
	//if (node->left || node->right)
	//	printf("\n");

	if (node->left) {
		printf("L(");
		tree_traverse(node->left);
		printf(") ");
	}
	if (node->right) {
		printf("R(");
		tree_traverse(node->right);
		printf(") ");
	}
}

float solve_traverse(node * node)
{
	if (node->val.type == INT)
		return (float)atoi(node->val.tok);
	float a, b, ans;
	if (node->left)
		a = solve_traverse(node->left);
	//printf("%s", node->val);
	if (node->right)
		b = solve_traverse(node->right);

	if (node->val.type == PLUS)
	{
		return a + b;
	}
	else if (node->val.type == MINUS)
	{
		return a - b;
	}
	else if (node->val.type == MULT)
	{
		return a * b;
	}
	else if (node->val.type == DIVIDE)
	{
		return a / b;
	}
	else if (node->val.type == GREATER)
	{
		return a > b;
	}
	else if (node->val.type == GREATER_OR_EQ)
	{
		return a >= b;
	}
	else if (node->val.type == LESS)
	{
		return a < b;
	}
	else if (node->val.type == LESS_OR_EQ)
	{
		return a <= b;
	}
	else if (node->val.type == IS_EQUAL)
	{
		return a == b;
	}
	else if (node->val.type == NOT_EQUAL)
	{
		return a != b;
	}

}

void val_traverse(node * node, instr_list * instrl, frame * currentframe)
{
	if (!node)
		return;
	int start_cond = 0;

	if (node->val.type == FUNCTION)
	{
		frame * nf = frame_init();

		nom_func * func = malloc(sizeof(nom_func));
		func->arg_count = node->num_branches - 1;
		func->frame = nf;
		nf->parent = currentframe;
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
		for (int i = 1; i < node->num_branches; i++)
		{
			if (node->branches[i])
				val_traverse(node->branches[i], instrl, currentframe);
		}
		int idx = get_var_index(currentframe, node->branches[0]->val.tok);
		if (idx == -1)
		{
			syntax_error(node->branches[0]->val.tok, node->branches[0]->val.col, node->branches[0]->val.row, "Undefined function");
			create_var(currentframe, node->branches[0]->val.tok, NUM);
			idx = currentframe->num_variables - 1;
		}

		push_instr(instrl, CALL, idx);
		return;
	}
	else if (node->val.type == INT)
	{
		nom_number * val = malloc(sizeof(nom_number));
		*val = (float)atoi(node->val.tok);
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
		for (int i = 0; i < size; i++)
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
	else if (node->val.type == IDENTIFIER)
	{
		int idx = get_var_index(currentframe, node->val.tok);
		if (idx == -1)
		{
			create_var(currentframe, node->val.tok, NUM);
			idx = currentframe->num_variables - 1;
		}
		push_instr(instrl, LOAD_NAME, idx);
		return;
	}
	else if (node->val.type == EQUAL)
	{
		//STORE_NAME
		//push_instr(instrl, );
		if (node->left->val.type != IDENTIFIER)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->left->val.tok);
			abort();
		}
		else
		{

			if (node->right)
				val_traverse(node->right, instrl, currentframe);

			int idx = get_var_index(currentframe, node->left->val.tok);
			if (idx == -1)
			{
				create_var(currentframe, node->left->val.tok, NONE);
				idx = currentframe->num_variables - 1;
			}
			push_instr(instrl, STORE_NAME, idx);
		}
		return;
	}
	else if (node->val.type == INC)
	{
		//STORE_NAME
		//push_instr(instrl, );
		if (node->next->val.type != IDENTIFIER)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->next->val.tok);
			abort();
		}
		else
		{

			int idx = get_var_index(currentframe, node->next->val.tok);
			if (idx == -1)
			{
				create_var(currentframe, node->next->val.tok, NONE);
				idx = currentframe->num_variables - 1;
			}
			push_instr(instrl, LOAD_NAME, idx);
			nom_number * val = malloc(sizeof(nom_number));
			*val = 1;
			push_instr(instrl, PUSH, add_const(currentframe, val));
			push_instr(instrl, ADD, 0);
			push_instr(instrl, STORE_NAME, idx);
		}
		return;
	}
	else if (node->val.type == DEC)
	{

		//STORE_NAME
		//push_instr(instrl, );
		if (node->next->val.type != IDENTIFIER)
		{
			printf("LEFT HAND VALUE %s IS NOT CONSTANT\n", node->next->val.tok);
			abort();
		}
		else
		{

			int idx = get_var_index(currentframe, node->next->val.tok);
			if (idx == -1)
			{
				create_var(currentframe, node->next->val.tok, NONE);
				idx = currentframe->num_variables - 1;
			}
			push_instr(instrl, LOAD_NAME, idx);
			nom_number * val = malloc(sizeof(nom_number));
			*val = -1;
			push_instr(instrl, PUSH, add_const(currentframe, val));
			push_instr(instrl, ADD, 0);
			push_instr(instrl, STORE_NAME, idx);
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
		instrl->instructions[idx].idx = (float)instrl->num_instructions;

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
		instrl->instructions[idx].idx = (float)instrl->num_instructions;

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
		instrl->instructions[idx].idx = (float)instrl->num_instructions+1;

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
		instrl->instructions[idx].idx = (float)instrl->num_instructions;
		instrl->instructions[idx2].idx = (float)instrl->num_instructions;

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
		instrl->instructions[idx].idx = (float)instrl->num_instructions + 1;
		instrl->instructions[idx2].idx = (float)instrl->num_instructions;
		return;
	}
	else if (node->val.type == ELSE)
	{
		start_cond = instrl->num_instructions;
		int idx = instrl->num_instructions;
		push_instr(instrl, JUMP, start_cond);
		if (node->next)
			val_traverse(node->next, instrl, currentframe);
		instrl->instructions[idx].idx = (float)instrl->num_instructions;

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
		instrl->instructions[idx].idx = (float)instrl->num_instructions;

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
	else if (node->val.type == GREATER)
	{
		push_instr(instrl, GTE, 0);
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

