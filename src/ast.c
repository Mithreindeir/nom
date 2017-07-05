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

#include "ast.h"

//Initialize a stack structure
op_stack * op_stack_init()
{
	op_stack * nstack = malloc(sizeof(op_stack));
	nstack->size = 0;
	nstack->stack = NULL;
	return nstack;
}
//Returns top node in stack
node * op_stack_gettop(op_stack * stack)
{
	if (stack->size <= 0)
		return NULL;
	node * top = stack->stack[stack->size - 1];
	return top;
}

//Pushes a node to top of the stack
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

//Pops top node from stack
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

//Initializes a node as empty
node * node_init()
{
	node * bo = malloc(sizeof(node));
	bo->type = EMPTY;
	return bo;
}

//Initialize a node as an operation
node * node_init_op(token tok)
{
	node * bo = malloc(sizeof(node));
	bo->val = tok;
	bo->right = NULL;
	bo->left = NULL;
	bo->type = LEAF;
	return bo;
}
//Initialize a node as a binary operator
node * node_init_binary(token  op, node * l, node * r)
{
	node * bo = malloc(sizeof(node));
	bo->val = op;
	bo->left = l;
	bo->right = r;
	bo->type = BINARY;
	return bo;
}

//initialize a node as unary operator
node * node_init_unary(token op, node * n)
{
	node * bo = malloc(sizeof(node));
	bo->val = op;
	bo->next = n;
	bo->type = UNARY;
	return bo;
}
//Create an AST from a token list, returning a root node
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

//Converts a code block into an AST taking line number and token list and returning root node as well as setting the end token
node * block_ast(token * tokens, int start, int num_tokens, int * tokens_used)
{
	int current_used = start;

	node * base = node_init();
	base->val.type = -1;
	base->type = MULTI;
	base->branches = NULL;
	base->num_branches = 0;

	int end_tok = start;
	int nested = 1;
	for (; end_tok < num_tokens; end_tok++)
	{
		if (tokens[end_tok].type == END)
			nested--;
		if (tokens[end_tok].type == COLON)
			nested++;
		if (nested <= 0 && tokens[end_tok+1].type != COLON)
			break;
	}

	if (tokens[end_tok].type != END && tokens[end_tok].type != ELSE)
	{
		syntax_error(tokens[start].tok, tokens[start].col, tokens[start].row, "NO END STATEMENT AFTER BLOCK");
	}

	while (current_used < end_tok)
	{
		node* branch = single_ast(tokens, current_used, end_tok, &current_used);

		base->num_branches++;
		if (base->num_branches == 1)
		{
			base->branches = malloc(sizeof(node*));
		}
		else
		{
			base->branches = realloc(base->branches, base->num_branches * sizeof(node*));
		}
		base->branches[base->num_branches - 1] = branch;
	}
	*tokens_used = current_used+1;

	return base;
}

//Converts single line into part of the AST 
node * single_ast(token * tokens, int start, int num_tokens, int * tokens_used)
{
	op_stack * operatorstack = op_stack_init();
	op_stack * expressionstack = op_stack_init();

	/*Variables for Syntax Errors
	-1 means no error
	0 means syntax found
	1 means throw error unless found
	Errors are handled at NEWLINE
	*/
	int colon_with_cond = -1;
	int colon_without_cond = 0;

	//Parentheses can mean a order of op thing, or for functions. This flag is on if it is for functions
	int func = 0;
	for (int i = start; i < num_tokens; i++)
	{
		token tok = tokens[i];
		*tokens_used = i+1;
		if (tok.type == LPAREN)
		{
			if (i > start)
			{
				token t2 = tokens[i - 1];
				if (t2.type == IDENTIFIER) {
					func = 1;
					tok.type = FUNC_CALL;
					tokens[i].type = FUNC_CALL;
					while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type == DOT) {
						node * top = op_stack_pop(operatorstack);

						int num_idxs = token_idxs(top->val);
						if (num_idxs == 2)
						{
							node * etop = op_stack_pop(expressionstack);
							node * netop = op_stack_pop(expressionstack);
							op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
						}
						free(top);
					}
				}
			}
			op_stack_push(operatorstack, node_init_op(tok));
		}
		else if (tok.type == LBRACK)
		{
			if (i > start)
			{
				token t2 = tokens[i - 1];
				if (t2.type == IDENTIFIER) {
					func = 1;
					tok.type = MEM_IDX;
					tokens[i].type = MEM_IDX;
					while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type == DOT) {
						node * top = op_stack_pop(operatorstack);

						int num_idxs = token_idxs(top->val);
						if (num_idxs == 2)
						{
							node * etop = op_stack_pop(expressionstack);
							node * netop = op_stack_pop(expressionstack);
							op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
						}
						free(top);
					}
				}
			}
			op_stack_push(operatorstack, node_init_op(tok));
		}
		else if (tok.type == COMMA)
		{
			if (operatorstack->size > 0 && !is_conditional(op_stack_gettop(operatorstack)->val) && op_stack_gettop(operatorstack)->val.type != LPAREN &&  op_stack_gettop(operatorstack)->val.type != FUNC_CALL)
			{
				node * top = op_stack_pop(operatorstack);

				int num_idxs = token_idxs(top->val);
				if (num_idxs == 2)
				{
					node * etop = op_stack_pop(expressionstack);
					node * netop = op_stack_pop(expressionstack);
					op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
				}
				else if (num_idxs == 1)
				{
					node * etop = op_stack_pop(expressionstack);
					op_stack_push(expressionstack, node_init_unary(top->val, etop));
				}
				free(top);
			}
		}
		else if (is_conditional(tok) || tok.type == FUNCTION)
		{
			colon_with_cond = 1;
			colon_without_cond = -1;
			op_stack_push(operatorstack, node_init_op(tok));
		}
		else if (tok.type == INT || tok.type == FLOAT || tok.type == IDENTIFIER || tok.type == STRING)
		{
			op_stack_push(expressionstack, node_init_op(tok));
		}
		else if (is_operator(tok))
		{
			while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type != FUNCTION && !is_conditional(op_stack_gettop(operatorstack)->val) && precedes(tok, op_stack_gettop(operatorstack)->val))
			{
				node * top = op_stack_pop(operatorstack);
				int num_idxs = token_idxs(top->val);

				if (num_idxs == 2)
				{
					node * etop = op_stack_pop(expressionstack);
					node * netop = op_stack_pop(expressionstack);

					op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));

				}
				else if (num_idxs == 1)
				{
					node * etop = op_stack_pop(expressionstack);
					op_stack_push(expressionstack, node_init_unary(top->val, etop));
				}

				free(top);
			}

			op_stack_push(operatorstack, node_init_op(tok));

		}
		else if (tok.type == COLON)
		{
			colon_with_cond = 0;
			colon_without_cond = colon_without_cond == 0 ? 1 : -1;
			while (operatorstack->size > 0 && !is_conditional(op_stack_gettop(operatorstack)->val) && op_stack_gettop(operatorstack)->val.type != FUNCTION)
			{
				node * top = op_stack_pop(operatorstack);
				int num_idxs = token_idxs(top->val);
				if (num_idxs == 2)
				{
					node * etop = op_stack_pop(expressionstack);
					node * netop = op_stack_pop(expressionstack);
					op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
				}
				else if (num_idxs == 1)
				{

					node * etop = op_stack_pop(expressionstack);
					op_stack_push(expressionstack, node_init_unary(top->val, etop));
				}
				free(top);
			}

			if (operatorstack->size > 0 && is_conditional(op_stack_gettop(operatorstack)->val)) {
				int f = i;
				node * cb = block_ast(tokens, i + 1, num_tokens, &f);
				i = f - 1;
				*tokens_used = f;
				//While and if are binary, For has 4 nodes (expr1,expr2,expr3) block
				if (op_stack_gettop(operatorstack)->val.type == FOR)
				{
					op_stack_gettop(operatorstack)->branches = malloc(sizeof(node*) * 4);
					op_stack_gettop(operatorstack)->num_branches = 4;
					op_stack_gettop(operatorstack)->type = MULTI;
					op_stack_gettop(operatorstack)->branches[3] = cb;
					op_stack_gettop(operatorstack)->branches[2] = op_stack_pop(expressionstack);
					op_stack_gettop(operatorstack)->branches[1] = op_stack_pop(expressionstack);
					op_stack_gettop(operatorstack)->branches[0] = op_stack_pop(expressionstack);
					op_stack_push(expressionstack, op_stack_pop(operatorstack));
				}
				else if (op_stack_gettop(operatorstack)->val.type == ELSE)
				{
					op_stack_gettop(operatorstack)->type = UNARY;
					op_stack_gettop(operatorstack)->next = cb;
					op_stack_push(expressionstack, op_stack_pop(operatorstack));
				}
				else  if (op_stack_gettop(operatorstack)->val.type == IF) 
				{
					if ((i+2) < num_tokens && (tokens[i + 2].type == ELSE || tokens[i + 2].type == ELSEIF)) op_stack_gettop(operatorstack)->val.type = IFELSE;
					op_stack_gettop(operatorstack)->type = BINARY;
					op_stack_gettop(operatorstack)->left = op_stack_pop(expressionstack);
					op_stack_gettop(operatorstack)->right = cb;
					op_stack_push(expressionstack, op_stack_pop(operatorstack));
				}
				else  if (op_stack_gettop(operatorstack)->val.type == ELSEIF)
				{
					if ((i + 2) < num_tokens && (tokens[i + 2].type == ELSE || tokens[i + 2].type == ELSEIF)) op_stack_gettop(operatorstack)->val.type = ELSEIFELSE;
					op_stack_gettop(operatorstack)->type = BINARY;
					op_stack_gettop(operatorstack)->left = op_stack_pop(expressionstack);
					op_stack_gettop(operatorstack)->right = cb;
					op_stack_push(expressionstack, op_stack_pop(operatorstack));
				}
				else {
					op_stack_gettop(operatorstack)->type = BINARY;
					op_stack_gettop(operatorstack)->left = op_stack_pop(expressionstack);
					op_stack_gettop(operatorstack)->right = cb;
					op_stack_push(expressionstack, op_stack_pop(operatorstack));
				}
			}
			else if (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type == FUNCTION)
			{

				int num_args = 0;
				int total_toks = 0;
				for (int j = i; j >= start; j--)
				{
					token t = tokens[j];
					if (t.type == FUNCTION)
						break;
					if (t.type == COMMA)
						num_args++;
					total_toks++;
				}
				if (total_toks > 3)
					num_args++;

				int f = i;
				node * cb = block_ast(tokens, i + 1, num_tokens, &f);
				i = f - 1;
				*tokens_used = f;

				op_stack_gettop(operatorstack)->num_branches = num_args + 1;
				op_stack_gettop(operatorstack)->branches = malloc(sizeof(node*) * (num_args+1));
				op_stack_gettop(operatorstack)->type = MULTI;

				op_stack_gettop(operatorstack)->branches[0] = cb;
				for (int i = 0; i < num_args && expressionstack->size > 0; i++)
				{
					op_stack_gettop(operatorstack)->branches[i+1] = op_stack_pop(expressionstack);
				}

				op_stack_push(expressionstack, op_stack_pop(operatorstack));
			}
		}
		else if (tok.type == RBRACK)
		{
			int nest = 0;
			func = 0;
			for (int j = i; j >= start; j--)
			{
				token t = tokens[j];
				if (t.type == MEM_IDX || t.type == LBRACK) {
					nest--;
					if (nest <= 0) {
						if (t.type == MEM_IDX)
							func = 1;
						else func = 0;
						break;
					}
				}
				if (t.type == RBRACK)
					nest++;
			}

			if (func)
			{
				while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type != MEM_IDX)
				{
					node * top = op_stack_pop(operatorstack);
					int num_idxs = token_idxs(top->val);
					if (num_idxs == 2)
					{
						node * etop = op_stack_pop(expressionstack);
						node * netop = op_stack_pop(expressionstack);
						op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
					}
					else if (num_idxs == 1)
					{
						node * etop = op_stack_pop(expressionstack);
						op_stack_push(expressionstack, node_init_unary(top->val, etop));
					}
					free(top);
				}

				node * func_call = op_stack_pop(operatorstack);

				int num_args = 0;
				int total_toks = 0;
				int nested = 0;
				for (int j = i; j >= start; j--)
				{
					token t = tokens[j];
					if (t.type == MEM_IDX || t.type == LBRACK)
						nested--;
					if (t.type == RBRACK)
						nested++;
					if (nested <= 0)
						break;
					if (t.type == COMMA && nested < 2)
						num_args++;
					total_toks++;
				}
				if (total_toks > 1)
					num_args++;
				func_call->type = BINARY;

				func_call->right = op_stack_pop(expressionstack);

				func_call->left = op_stack_pop(expressionstack);
				op_stack_push(expressionstack, func_call);
				func = 0;
			}
			else {
				while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type != LBRACK)
				{
					node * top = op_stack_pop(operatorstack);
					int num_idxs = token_idxs(top->val);
					if (num_idxs == 2)
					{
						node * etop = op_stack_pop(expressionstack);
						node * netop = op_stack_pop(expressionstack);

						op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
					}
					else if (num_idxs == 1)
					{

						node * etop = op_stack_pop(expressionstack);
						op_stack_push(expressionstack, node_init_unary(top->val, etop));
					}

					free(top);
				}
				if (operatorstack->size == 0)
				{
					syntax_error(tokens[i].tok, tokens[i].col, tokens[i].row, "NO MATCHING PARENTHESES BLOCK");
				}
				op_stack_pop(operatorstack);
			}
		}
		else if (tok.type == RPAREN)
		{
			int nest = 0;
			func = 0;
			for (int j = i; j >= start; j--)
			{
				token t = tokens[j];
				if (t.type == FUNC_CALL || t.type == LPAREN) {
					nest--;
					if (nest <= 0) {
						if (t.type == FUNC_CALL)
							func = 1;
						else func = 0;
						break;
					}
				}
				if (t.type == RPAREN)
					nest++;
			}

			if (func)
			{
				while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type != FUNC_CALL)
				{
					node * top = op_stack_pop(operatorstack);
					int num_idxs = token_idxs(top->val);
					if (num_idxs == 2)
					{
						node * etop = op_stack_pop(expressionstack);
						node * netop = op_stack_pop(expressionstack);
						op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
					}
					else if (num_idxs == 1)
					{
						node * etop = op_stack_pop(expressionstack);
						op_stack_push(expressionstack, node_init_unary(top->val, etop));
					}
					free(top);
				}

				node * func_call = op_stack_pop(operatorstack);
			
				int num_args = 0;
				int total_toks = 0;
				int nested = 0;
				for (int j = i; j >= start; j--)
				{
					token t = tokens[j];
					if (t.type == FUNC_CALL || t.type == LPAREN)
						nested--;
					if (t.type == RPAREN)
						nested++;
					if (nested <= 0)
						break;
					if (t.type == COMMA && nested < 2)
						num_args++;
					total_toks++;
				}
				if (total_toks > 1)
					num_args++;
				func_call->num_branches = num_args + 1;
				func_call->branches = malloc(sizeof(node*) * (num_args + 1));
				func_call->type = MULTI;

				for (int i = 0; i < num_args && expressionstack->size > 0; i++)
				{
					func_call->branches[i + 1] = op_stack_pop(expressionstack);
				}

				func_call->branches[0] = op_stack_pop(expressionstack);
				op_stack_push(expressionstack, func_call);
				func = 0;
			}
			else {
				while (operatorstack->size > 0 && op_stack_gettop(operatorstack)->val.type != LPAREN)
				{
					node * top = op_stack_pop(operatorstack);
					int num_idxs = token_idxs(top->val);
					if (num_idxs == 2)
					{
						node * etop = op_stack_pop(expressionstack);
						node * netop = op_stack_pop(expressionstack);

						op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));
					}
					else if (num_idxs == 1)
					{

						node * etop = op_stack_pop(expressionstack);
						op_stack_push(expressionstack, node_init_unary(top->val, etop));
					}

					free(top);
				}
				if (operatorstack->size == 0)
				{
					syntax_error(tokens[i].tok, tokens[i].col, tokens[i].row, "NO MATCHING PARENTHESES BLOCK");
				}
				op_stack_pop(operatorstack);
			}
		}
		else if (tok.type == NEWLINE && i != start)
		{
			if (colon_with_cond == 1)
			{
				syntax_error(tokens[i].tok, tokens[i].col, tokens[i].row, "NO COLON AFTER EXPR FOR CONDITIONAL OR LOOP");
			}
			if (colon_without_cond == 1)
			{
				syntax_error(tokens[i].tok, tokens[i].col, tokens[i].row, "COLON BUT NO CONDITIONAL OR LOOP");
			}
			break;
		}
	}
	while (operatorstack->size > 0)
	{
		node * top = op_stack_pop(operatorstack);
		int num_idxs = token_idxs(top->val);

		if (expressionstack->size < num_idxs)
		{
			syntax_error(top->val.tok, top->val.col, top->val.row, "INCORRECT OPERATOR OPERANDS");
		}
		if (num_idxs == 2)
		{
			node * etop = op_stack_pop(expressionstack);
			node * netop = op_stack_pop(expressionstack);
			op_stack_push(expressionstack, node_init_binary(top->val, netop, etop));

		}
		else if (num_idxs == 1)
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

//Operator precedence. returns bool of which token precedes
int precedes(token tok1, token tok2)
{
	int lass = token_associative(tok1);
	return ((lass && token_precedence(tok1) <= token_precedence(tok2)) || (!lass && token_precedence(tok1) < token_precedence(tok2)));
}

//Frees memory in the tree given root node
void free_nodes(node * root)
{
	if (!root)
		return;
	if (root->type == BINARY)
	{
		free_nodes(root->left);
		free_nodes(root->right);
		root->type = EMPTY;
	}
	else if (root->type == UNARY)
	{
		free_nodes(root->next);
		root->type = EMPTY;
	}
	else if (root->type == MULTI)
	{
		for (int i = 0; i < root->num_branches; i++)
		{
			if (root->branches[i])
				free_nodes(root->branches[i]);
		}
		if (root->branches) free(root->branches);
		root->type = EMPTY;
	}
	else if (root->type == LEAF)
	{
		free_nodes(root->next);
		root->type = EMPTY;
	}

	if (root->type == EMPTY)
	{
		free(root);
		root = NULL;
	}
}
