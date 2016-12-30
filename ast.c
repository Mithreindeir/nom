#include "ast.h"

op_stack * op_stack_init()
{
	op_stack * nstack = malloc(sizeof(op_stack));
	nstack->size = 0;
	nstack->stack = NULL;
	return nstack;
}

node * op_stack_gettop(op_stack * stack)
{
	if (stack->size <= 0)
		return NULL;
	node * top = stack->stack[stack->size - 1];
	return top;
}

void op_stack_push(op_stack * stack, node * nnode)
{
	stack->size++;
	if (stack->size == 1)
	{
		stack->stack = malloc(sizeof(node*) * stack->size);
	}
	else
	{
		stack->stack = realloc(stack->stack, sizeof(node*) * stack->size);
	}
	stack->stack[stack->size - 1] = nnode;
}

node * op_stack_pop(op_stack * stack)
{
	if (stack->size <= 0)
		return NULL;
	node * top = stack->stack[stack->size - 1];
	stack->size--;
	if (stack->size == 0)
	{
		free(stack->stack);
		stack->stack = NULL;
	}
	else
	{
		stack->stack = realloc(stack->stack, sizeof(node*) * stack->size);
	}
	return top;
}

node * node_init()
{
	node * bo = malloc(sizeof(node));
	bo->type = EMPTY;
	return bo;
}

node * node_init_op(token tok)
{
	node * bo = malloc(sizeof(node));
	bo->val = tok;
	bo->right = NULL;
	bo->left = NULL;
	bo->type = LEAF;
	return bo;
}

node * node_init_binary(token  op, node * l, node * r)
{
	node * bo = malloc(sizeof(node));
	bo->val = op;
	bo->left = l;
	bo->right = r;
	bo->type = BINARY;
	return bo;
}

node * node_init_unary(token op, node * n)
{
	node * bo = malloc(sizeof(node));
	bo->val = op;
	bo->next = n;
	bo->type = UNARY;
	return bo;
}

node * parse_string(token * tokens, int num_tokens)
{
	int current_used = 0;
	node * base = node_init();
	base->val.type = -1;
	base->type = MULTI;
	base->branches = NULL;
	base->num_branches = 0;

	while (current_used < num_tokens)
	{
		node* branch = single_ast(tokens, current_used, num_tokens, &current_used);

		base->num_branches++;
		if (base->num_branches == 1)
		{
			base->branches = malloc(sizeof(node*));
		}
		else
		{
			base->branches = realloc(base->branches, base->num_branches * sizeof(node*));
		}
		base->branches[base->num_branches-1] = branch;
	}

	//node * root = single_ast(tokens, num_tokens);
	return base;
}

node * single_ast(token * tokens, int start, int num_tokens, int * tokens_used)
{
	op_stack * operatorstack = op_stack_init();
	op_stack * expressionstack = op_stack_init();
	for (int i = start; i < num_tokens; i++)
	{
		token tok = tokens[i];
		*tokens_used = i+1;

		if (tok.type == LPAREN)
		{
			op_stack_push(operatorstack, node_init_op(tok));
		}
		else if (tok.type == INT || tok.type == FLOAT || tok.type == IDENTIFIER)
		{
			op_stack_push(expressionstack, node_init_op(tok));
		}
		else if (is_operator(tok))
		{
			while (operatorstack->size > 0 && precedes(tok, op_stack_gettop(operatorstack)->val))
			{
				node * top = op_stack_pop(operatorstack);
				node * etop = op_stack_pop(expressionstack);
				node * netop = op_stack_pop(expressionstack);
				op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
				free(top);
			}
			op_stack_push(operatorstack, node_init_op(tok));

		}
		else if (tok.type == RPAREN)
		{
			while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type != LPAREN)
			{
				node * top = op_stack_pop(operatorstack);
				int num_operands = token_operands(top->val);
				if (num_operands == 2)
				{
					node * etop = op_stack_pop(expressionstack);
					node * netop = op_stack_pop(expressionstack);

					op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
				}
				else if (num_operands == 1)
				{

					node * etop = op_stack_pop(expressionstack);
					op_stack_push(expressionstack, node_init_unary(top->val, etop));
				}

				free(top);
			}

			op_stack_pop(operatorstack);
		}
		else if (tok.type == NEWLINE)
		{
			break;
		}

	}
	while (operatorstack->size > 0)
	{
		node * top = op_stack_pop(operatorstack);
		int num_operands = token_operands(top->val);
		if (num_operands == 2)
		{
			node * etop = op_stack_pop(expressionstack);
			node * netop = op_stack_pop(expressionstack);

			op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
		}
		else if (num_operands == 1)
		{
			node * etop = op_stack_pop(expressionstack);
			op_stack_push(expressionstack, node_init_unary(top->val, etop));
		}
		free(top);
	}
	node * root = op_stack_pop(expressionstack);
	if (operatorstack->stack) free(operatorstack->stack);
	free(operatorstack);
	if (expressionstack->stack) free(expressionstack->stack);
	free(expressionstack);
	return root;
}

int precedes(token tok1, token tok2)
{
	int lass = token_associative(tok1);
	return ((lass && token_precedence(tok1) <= token_precedence(tok2)) || (!lass && token_precedence(tok1) < token_precedence(tok2)));
}

void free_nodes(node * root)
{
	if (!root)
		return;
	if (root->type == BINARY)
	{
		free_nodes(root->left);
		free_nodes(root->right);
		root->type == EMPTY;
	}
	else if (root->type == UNARY)
	{
		free_nodes(root->next);
		root->type == EMPTY;
	}
	else if (root->type == MULTI)
	{
		for (int i = 0; i < root->num_branches; i++)
		{
			if (root->branches[i])
				free_nodes(root->branches[i]);
		}
		if (root->branches) free(root->branches);
		root->type == EMPTY;
	}

	if (root->type == EMPTY)
	{
		free(root);
		root = NULL;
	}
}
