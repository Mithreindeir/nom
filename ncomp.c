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
	c.operand = op;
	instrl->instructions[instrl->num_instructions - 1] = c;
}

instr_list * compile(node * bop, cinterp * cinterpreter)
{
	instr_list * instrl = malloc(sizeof(instr_list));
	instrl->num_instructions = 0;
	instrl->instructions = NULL;
	val_traverse(bop, instrl, cinterpreter);
	push_instr(instrl, PRINT, 0);
	//traverse(bop);
	//printf("=%f\n", solve_traverse(bop));
	//tree_traverse(bop);

	//getch();
	free_nodes(bop);
	return instrl;
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

void val_traverse(node * node, instr_list * instrl, cinterp * cinterpreter)
{
	if (!node)
		return;
	int start_cond = 0;
	//printf("%s\n", node->val.tok);

	if (node->val.type == INT)
	{
		push_instr(instrl, PUSH, (float)atoi(node->val.tok));
		return;
	}
	else if (node->val.type == FLOAT)
	{
		push_instr(instrl, PUSH, atof(node->val.tok));
		return;
	}
	else if (node->val.type == IDENTIFIER)
	{
		int idx = get_var_index(cinterpreter, node->val.tok);
		if (idx == -1)
		{
			create_var(cinterpreter, node->val.tok, NUM);
			idx = cinterpreter->num_variables - 1;
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
				val_traverse(node->right, instrl, cinterpreter);

			int idx = get_var_index(cinterpreter, node->left->val.tok);
			if (idx == -1)
			{
				create_var(cinterpreter, node->left->val.tok, NUM);
				idx = cinterpreter->num_variables - 1;
			}

			push_instr(instrl, STORE_NAME, idx);
		}
		return;
	}
	else if (node->val.type == WHILE)
	{
		start_cond = instrl->num_instructions;
		if (node->left)
			val_traverse(node->left, instrl, cinterpreter);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->right)
			val_traverse(node->right, instrl, cinterpreter);
		push_instr(instrl, JUMP, start_cond);
		instrl->instructions[idx].operand = (float)instrl->num_instructions;

		return;
	}
	else if (node->val.type == IF)
	{
		start_cond = instrl->num_instructions;
		if (node->left)
			val_traverse(node->left, instrl, cinterpreter);
		int idx = instrl->num_instructions;
		push_instr(instrl, IFEQ, start_cond);
		if (node->right)
			val_traverse(node->right, instrl, cinterpreter);
		instrl->instructions[idx].operand = (float)instrl->num_instructions;

		return;
	}

	if (node->type == BINARY)
	{
		if (node->left)
			val_traverse(node->left, instrl, cinterpreter);
		if (node->right)
			val_traverse(node->right, instrl, cinterpreter);
	}
	else if (node->type == UNARY)
	{
		if (node->next)
			val_traverse(node->next, instrl, cinterpreter);
	}
	else if (node->type == MULTI)
	{
		for (int i = 0; i < node->num_branches; i++)
		{
			if (node->branches[i])
				val_traverse(node->branches[i], instrl, cinterpreter);
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
}

