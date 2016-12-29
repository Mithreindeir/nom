#include "ast.h"

op_stack * op_stack_init()
{
	op_stack * nstack = malloc(sizeof(op_stack));
	nstack->size = 0;
	nstack->stack = NULL;
	return nstack;
}

binop * op_stack_gettop(op_stack * stack)
{
	if (stack->size <= 0)
		return NULL;
	binop * top = stack->stack[stack->size - 1];
	return top;
}

void op_stack_push(op_stack * stack, binop * nbinop)
{
	stack->size++;
	if (stack->size == 1)
	{
		stack->stack = malloc(sizeof(binop*) * stack->size);
	}
	else
	{
		stack->stack = realloc(stack->stack, sizeof(binop*) * stack->size);
	}
	stack->stack[stack->size - 1] = nbinop;
}

binop * op_stack_pop(op_stack * stack)
{
	if (stack->size <= 0)
		return NULL;
	binop * top = stack->stack[stack->size - 1];
	stack->size--;
	if (stack->size == 0)
	{
		free(stack->stack);
	}
	else
	{
		stack->stack = realloc(stack->stack, sizeof(binop*) * stack->size);
	}
	return top;
}

num num_init(char * tok)
{
	num n;
	n.num = tok;
	n.value = atoi(tok);
	return n;
}

binop * binop_init()
{
	binop * bo = malloc(sizeof(binop));
	bo->right = NULL;
	bo->left = NULL;
	return bo;
}

binop * binop_init_op(token tok)
{
	binop * bo = malloc(sizeof(binop));
	bo->val = tok;
	bo->right = NULL;
	bo->left = NULL;
	return bo;
}

binop * binop_init_branch(token  op, binop * l, binop * r)
{
	binop * bo = malloc(sizeof(binop));
	bo->val = op;
	bo->left = l;
	bo->right = r;
	return bo;
}

binop * parse_string(token * tokens, int num_tokens)
{
	op_stack * operatorstack = op_stack_init();
	op_stack * expressionstack = op_stack_init();
	for (int i = 0; i < num_tokens; i++)
	{
		token tok = tokens[i];
		if (tok.type == LPAREN)
		{
			op_stack_push(operatorstack, binop_init_op(tok));
		}
		else if (tok.type == INT || tok.type == FLOAT || tok.type == IDENTIFIER)
		{
			op_stack_push(expressionstack, binop_init_op(tok));
		}
		else if (is_operator(tok))
		{
			while (operatorstack->size > 0 && precedes(tok, op_stack_gettop(operatorstack)->val))
			{
				binop * top = op_stack_pop(operatorstack);
				binop * etop = op_stack_pop(expressionstack);
				binop * netop = op_stack_pop(expressionstack);
				op_stack_push(expressionstack, binop_init_branch(top->val, netop, etop));
				free(top);
			}
			op_stack_push(operatorstack, binop_init_op(tok));
			
		}
		else if (tok.type == RPAREN)
		{
			while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type != LPAREN)
			{				
				binop * top = op_stack_pop(operatorstack);
				binop * etop = op_stack_pop(expressionstack);
				binop * netop = op_stack_pop(expressionstack);

				op_stack_push(expressionstack, binop_init_branch(top->val, netop, etop));

				free(top);
			}

			op_stack_pop(operatorstack);
		}
		else return;

	}
	while (operatorstack->size > 0)
	{
		binop * top = op_stack_pop(operatorstack);
		binop * etop = op_stack_pop(expressionstack);
		binop * netop = op_stack_pop(expressionstack);
		//printf("%s and %s and %s\n", top->val.tok, etop->val.tok, netop->val.tok);
		op_stack_push(expressionstack, binop_init_branch(top->val, netop, etop));
		free(top);
	}
	binop * root = op_stack_pop(expressionstack);
	//printf("=%d\n", val_traverse(root));
	return root;
}

int precedes(token tok1, token tok2)
{
	int lass = token_associative(tok1);
	return ((lass && token_precedence(tok1) <= token_precedence(tok2)) || (!lass && token_precedence(tok1) < token_precedence(tok2)));
}
