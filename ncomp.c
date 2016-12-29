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

instr_list * compile(binop * bop, cinterp * cinterpreter)
{
	instr_list * instrl = malloc(sizeof(instr_list));
	instrl->num_instructions = 0;
	instrl->instructions = NULL;
	val_traverse(bop, instrl, cinterpreter);
	push_instr(instrl, PRINT, 0);
	traverse(bop);
	//printf("=%f\n", solve_traverse(bop));
	//tree_traverse(bop);

	getch();
	return instrl;
}


void traverse(binop * binop)
{
	if (binop == NULL)
		return;
	traverse(binop->left);
	traverse(binop->right);

	printf("%s ", binop->val.tok);
}
void tree_traverse(binop * binop)
{
	printf(" %s ", binop->val.tok);
	//if (binop->left || binop->right)
	//	printf("\n");

	if (binop->left) {
		printf("L(");
		tree_traverse(binop->left);
		printf(") ");
	}
	if (binop->right) {
		printf("R(");
		tree_traverse(binop->right);
		printf(") ");
	}
}

float solve_traverse(binop * binop)
{
	if (binop->val.type == INT)
		return (float)atoi(binop->val.tok);
	float a, b, ans;
	if (binop->left)
		a = solve_traverse(binop->left);
	//printf("%s", binop->val);
	if (binop->right)
		b = solve_traverse(binop->right);

	if (binop->val.type == PLUS)
	{
		return a + b;
	}
	else if (binop->val.type == MINUS)
	{
		return a - b;
	}
	else if (binop->val.type == MULT)
	{
		return a * b;
	}
	else if (binop->val.type == DIVIDE)
	{
		return a / b;
	}
	else if (binop->val.type == GREATER)
	{
		return a > b;
	}
	else if (binop->val.type == GREATER_OR_EQ)
	{
		return a >= b;
	}
	else if (binop->val.type == LESS)
	{
		return a < b;
	}
	else if (binop->val.type == LESS_OR_EQ)
	{
		return a <= b;
	}
	else if (binop->val.type == IS_EQUAL)
	{
		return a == b;
	}
	else if (binop->val.type == NOT_EQUAL)
	{
		return a != b;
	}

}

void val_traverse(binop * binop, instr_list * instrl, cinterp * cinterpreter)
{
	if (binop->val.type == INT)
	{
		push_instr(instrl, PUSH, (float)atoi(binop->val.tok));
		return;
	}
	else if (binop->val.type == FLOAT)
	{
		push_instr(instrl, PUSH, atof(binop->val.tok));
		return;
	}
	else if (binop->val.type == IDENTIFIER)
	{
		int idx = get_var_index(cinterpreter, binop->val.tok);
		if (idx == -1)
		{
			create_var(cinterpreter, binop->val.tok, NUM);
			idx = cinterpreter->num_variables - 1;
		}
		
		push_instr(instrl, LOAD_NAME, idx);
		return;
	}
	else if (binop->val.type == EQUAL)
	{
		//STORE_NAME
		//push_instr(instrl, );
		if (binop->left->val.type != IDENTIFIER)
		{
			printf("LEFT HAND VALUE MUST BE CONSTANT\n");
			abort();
		}
		else
		{

			if (binop->right)
				val_traverse(binop->right, instrl, cinterpreter);

			int idx = get_var_index(cinterpreter, binop->left->val.tok);
			if (idx == -1)
			{
				create_var(cinterpreter, binop->left->val.tok, NUM);
				idx = cinterpreter->num_variables - 1;
			}

			push_instr(instrl, STORE_NAME, idx);
			push_instr(instrl, LOAD_NAME, idx);

		}
		return;
	}
	if (binop->left)
		val_traverse(binop->left, instrl, cinterpreter);
	if (binop->right)
		val_traverse(binop->right, instrl, cinterpreter);

	if (binop->val.type == PLUS)
	{
		push_instr(instrl, ADD, 0);
		return;
	}
	else if (binop->val.type == MINUS)
	{
		push_instr(instrl, SUB, 0);
		return;
	}
	else if (binop->val.type == MULT)
	{
		push_instr(instrl, MUL, 0);
		return;
	}
	else if (binop->val.type == DIVIDE)
	{
		push_instr(instrl, DIV, 0);
		return;
	}
	else if (binop->val.type == GREATER)
	{
		push_instr(instrl, GTE, 0);
		return;
	}
	else if (binop->val.type == GREATER_OR_EQ)
	{
		push_instr(instrl, GTE, 0);
		return;
	}
	else if (binop->val.type == LESS)
	{
		push_instr(instrl, LT, 0);
		return;
	}
	else if (binop->val.type == LESS_OR_EQ)
	{
		push_instr(instrl, LTE, 0);
		return;
	}
	else if (binop->val.type == IS_EQUAL)
	{
		push_instr(instrl, EQ, 0);
		return;
	}
	else if (binop->val.type == NOT_EQUAL)
	{
		push_instr(instrl, NE, 0);
		return;
	}
}

