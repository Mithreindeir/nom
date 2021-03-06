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

#include "clexer.h"
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifdef max
#undef max
#endif

int max(int a, int b)
{
	return a > b ? a : b;
}
int tcidx = 0;
int col = 0, row = 1;
//Separates into tokens
char * strtokm(char * s)
{
	if (!s || s[tcidx] == '\0')
		return NULL;

	char * f = malloc(sizeof(char) * strlen(s)+1);
	int i = tcidx, k = 0;
	//Ignore spaces and tabs
	while (s[i] == ' ' || s[i] == '\t') {
		i++;
		col++;
	}
	if (s[i] == '\0')
	{
		free(f);
		return NULL;
	}
	//If it is in quotations, keep whitespace
	int quotations = 0;
	int comment = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '"')
			quotations = !quotations;
		if (!quotations && s[i]=='#')
			comment = 1;
		if (s[i] == '\n') comment = 0;
		col++;
		if (s[i] == ';' && quotations == 0) {
			s[i] = '\n';
		}
		if (s[i] == '\n')
		{
			col = 0;
			row++;
		}

		if (!comment && !quotations && (s[i] == ' ' || s[i] == '\t'))
		{
			f[k] = 0;
			tcidx = i;
			return f;
		}
		else {
			f[k] = s[i];
		}
		k++;
		i++;
	}
	tcidx = i;
	f[k] = 0;
	return f;
}
//The type of the last token for context related idxs (eg: unary_neg vs binary_sub)
int last_tok_type = -1;
//Tokenizes the string and sets the type of token
token * tokenize(char * file, int * num_tok)
{
	tcidx = 0;
	char * tok = strtokm(file);
	int num_tokens = 0;
	token * tokens = NULL;
	int current_idx = 0;
	int comment = 0;
	while (tok != NULL)
	{

		int tlen = strlen(tok);
		int len = 0;
		int type = token_type(tok, &len);

		if (len == 0)
		{
			tok += 1;
			current_idx += 1;

			continue;
		}

		if (type != COMMENT)
		{
			token ctoken;
			ctoken.col = col;
			ctoken.row = row;
			if (type == STRING)
			{
				ctoken.tok = malloc(len - 1);
				ctoken.len = len-1;
			}
			else {
				ctoken.tok = malloc(len + 1);
				ctoken.len = len+1;
			}
			ctoken.type = type;
			last_tok_type = type;
			if (type == STRING)
			{
				int i = 0;
				while (i < (len - 2))
				{
					ctoken.tok[i] = tok[i + 1];
					i++;
				}
				ctoken.tok[i] = '\0';
			}
			else {
				int i = 0;
				while (i < len)
				{
					ctoken.tok[i] = tok[i];
					i++;
				}
				ctoken.tok[i] = '\0';
			}
			num_tokens++;
			if (num_tokens == 1)
			{
				tokens = malloc(sizeof(token) * num_tokens);
			}
			else
			{
				tokens = realloc(tokens, sizeof(token) * num_tokens);
			}
			tokens[num_tokens - 1] = ctoken;
		}
		if (tlen <= len) {
			free((tok - current_idx));
			tok = strtokm(file);
			current_idx = 0;
		}
		else {
			tok += len;
			current_idx += len;
		}
	}
	*num_tok = num_tokens;
	return tokens;
}

//Returns token type given token and sets length
int token_type(char * tok, int * len)
{
	int tlen = strlen(tok);

	*len = 0;
	if (!strncmp(tok, "if", max(2,tlen)))
	{
		*len = 2;
		return IF;
	}
	if (!strncmp(tok, "var", max(3,tlen)))
	{
		*len = 3;
		return VAR;
	}
	else if (!strncmp(tok, "else", max(4, tlen)) || !strncmp(tok, "else:", 5))
	{
		*len = 4;
		return ELSE;
	}
	else if (!strncmp(tok, "elseif", max(6, tlen)))
	{
		*len = 6;
		return ELSEIF;
	}
	else if (!strncmp(tok, "end", max(3, tlen)) || !strncmp(tok, "end\n", 4))
	{
		*len = 3;
		return END;
	}
	else if (!strncmp(tok, "return", max(6, tlen)) || !strncmp(tok, "return\n", 7))
	{
		*len = 6;
		return RETURN;
	}
	else if (!strncmp(tok, "for", max(3, tlen)))
	{
		*len = 3;
		return FOR;
	}
	else if (!strncmp(tok, "while", max(5, tlen)))
	{
		*len = 5;
		return WHILE;
	}
	else if (!strncmp(tok, "function", max(8, tlen)) || !strncmp(tok, "function(", 9))
	{
		*len = 8;
		return FUNCTION;
	}
	else if (*tok == '#')
	{
		int i = 0;
		char c = *(tok+1);
		while (c != '\0' && c != '\n')
		{
			i++;
			c = *(tok + i + 1);
		}
		*len = i+2;
		return COMMENT;
	}
	else if (!strncmp(tok, "or", max(2, tlen)))
	{
		*len = 2;
		return LOR;
	}
	else if (!strncmp(tok, "nor", max(3, tlen)))
	{

		*len = 3;
		return LNOR;
	}
	else if (!strncmp(tok, "and", max(3, tlen)))
	{
		
		*len = 3;
		return LAND;
	}
	else if (!strncmp(tok, "not", max(3, tlen)))
	{
		*len = 3;
		return LNOT;
	}
	else if (!strncmp(tok, "nand", max(4, tlen)))
	{
		*len = 4;
		return LNAND;
	}

	else if (!strncmp(tok, "==", 2))
	{
		*len = 2;
		return IS_EQUAL;
	}
	else if (!strncmp(tok, "=", 1))
	{
		*len = 1;
		return EQUAL;
	}
	else if (!strncmp(tok, "!=", 2))
	{
		*len = 2;
		return NOT_EQUAL;
	}
	else if (!strncmp(tok, ">=", 2))
	{
		*len = 2;
		return GREATER_OR_EQ;
	}
	else if (!strncmp(tok, ">", 1))
	{
		*len = 1;
		return GREATER;
	}
	else if (!strncmp(tok, "<=", 2))
	{
		*len = 2;
		return LESS_OR_EQ;
	}
	else if (!strncmp(tok, "<", 1))
	{
		*len = 1;
		return LESS;
	}
	else if (!strncmp(tok, ")", 1))
	{
		*len = 1;
		return RPAREN;
	}
	else if (!strncmp(tok, "(", 1))
	{
		*len = 1;
		return LPAREN;
	}
	else if (!strncmp(tok, "]", 1))
	{
		*len = 1;
		return RBRACK;
	}
	else if (!strncmp(tok, "[", 1))
	{
		*len = 1;
		return LBRACK;
	}
	else if (!strncmp(tok, "}", 1))
	{
		*len = 1;
		return RCBRACK;
	}
	else if (!strncmp(tok, "{", 1))
	{
		*len = 1;
		return LCBRACK;
	}
	else if (*tok == '"')
	{
		int i = 0;
		char c = *(tok+1);
		while (c != '\0' && c != '"')
		{
			i++;
			c = *(tok + i + 1);
		}
		if (c == '\0')
			syntax_error(tok, col, row, "NO ENDING QUOTATION");
		*len = i+2;
		return STRING;
	}
	else if (*tok >= 0x30 && *tok <= 0x39)
	{

		int i = 0;
		int t = INT;
		char c = *tok;
		while (c != '\0' && ((c >= 0x30 && c <= 0x39) || (c == '.')))
		{
			if (c == '.')
				t = FLOAT;
			i++;
			c = *(tok + i);
		}
		*len = i;
		return t;
	}
	else if (!strncmp(tok, ":", 1))
	{
		*len = 1;
		return COLON;
	}
	else if (!strncmp(tok, ",", 1))
	{
		*len = 1;
		return COMMA;
	}
	else if (!strncmp(tok, "++", 2))
	{
		*len = 2;
		return INC;
	}
	else if (!strncmp(tok, "+", 1))
	{
		*len = 1;
		return PLUS;
	}
	else if (!strncmp(tok, "--", 2))
	{

		if (last_tok_type == IDENTIFIER)
		{
			*len = 2;
			return DEC;
		}
		else
		{
			*len = 1;
			return MINUS;
		}
	}
	else if (!strncmp(tok, "-", 1))
	{
		if (last_tok_type == INT || last_tok_type == FLOAT || last_tok_type == IDENTIFIER || last_tok_type == RPAREN ||  last_tok_type == RBRACK)
		{
			*len = 1;
			return MINUS;
		}
		else
		{
			*len = 1;
			return UNARY_NEG;
		}
	}
	else if (!strncmp(tok, "*", 1))
	{
		*len = 1;
		return MULT;
	}
	else if (!strncmp(tok, "%", 1))
	{
		*len = 1;
		return MODULUS;
	}
	else if (!strncmp(tok, ".", 1))
	{
		*len = 1;
		return DOT;
	}
	else if (!strncmp(tok, "/", 1))
	{
		*len = 1;
		return DIVIDE;
	}
	else if (!strncmp(tok, "&", 1))
	{
		*len = 1;
		return BAND;
	}
	else if (!strncmp(tok, "|", 1))
	{
		*len = 1;
		return BOR;
	}
	else if (!strncmp(tok, "^", 1))
	{
		*len = 1;
		return BXOR;
	}
	else if (*tok == '\n')
	{
		*len = 1;
		return NEWLINE;
	}
	else if (!(*tok >= 0x30 && *tok <= 0x39))
	{
		int i = 0;
		char c = *tok;
		while (i < tlen && ((c >= 0x30 && c <= 0x39) || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'))
		{
			i++;
			c = *(tok + i);
		}
		
		*len = i;
		return IDENTIFIER;
	}
	return -1;
}

//Returns if the token is an operator
int is_operator(token tok)
{
	if (tok.type == PLUS || tok.type == MINUS || tok.type == DIVIDE || tok.type == MULT || tok.type == MODULUS)
		return 1;
	if (tok.type == DOT)
		return 1;
	if (tok.type == LESS || tok.type == GREATER || tok.type == LESS_OR_EQ || tok.type == GREATER_OR_EQ || tok.type == IS_EQUAL || tok.type == NOT_EQUAL)
		return 1;
	if (tok.type == EQUAL)
		return 1;
	if (tok.type == VAR || tok.type == UNARY_NEG || tok.type == INC || tok.type == DEC || tok.type == LNOT)
		return 1;
	if (tok.type == LAND || tok.type == LNAND || tok.type == LNOR || tok.type == LOR)
		return 1;
	if (tok.type == RETURN)
		return 1;
	return 0;
}

//Returns if the token is a conditional/branch
int is_conditional(token tok)
{
	if (tok.type == WHILE || tok.type == FOR || tok.type == IF || tok.type == ELSE || tok.type == ELSEIF)
		return 1;
	return 0;
}

//Returns the a number corresponding to the priority/precedence of the token
int token_precedence(token tok)
{
	if (tok.type == EQUAL)
		return -2;
	if (tok.type == RETURN)
		return -2;
	if (tok.type == LAND || tok.type == LNAND || tok.type == LNOR || tok.type == LOR)
		return -1;
	if (tok.type == LESS || tok.type == GREATER || tok.type == LESS_OR_EQ || tok.type == GREATER_OR_EQ || tok.type == IS_EQUAL || tok.type == NOT_EQUAL)
		return 0;
	if (tok.type == PLUS || tok.type == MINUS)
		return 1;
	if (tok.type == MULT || tok.type == DIVIDE || tok.type == MODULUS)
		return 2;
	if (tok.type == VAR || tok.type == UNARY_NEG || tok.type == INC || tok.type == DEC || tok.type == LNOT)
		return 3;
	if (tok.type == DOT)
		return 4;
	return -3;
}

//Is the token right or left associative
int token_associative(token tok)
{
	if (tok.type == VAR || tok.type == UNARY_NEG || tok.type == LNOT)
		return 0;
	if (tok.type == RETURN)
		return 0;
	return 1;
}

//Returns the index of the token
int token_idxs(token tok)
{
	if (tok.type == PLUS || tok.type == MINUS)
		return 2;
	if (tok.type == DOT)
		return 2;
	if (tok.type == MULT || tok.type == DIVIDE || tok.type == MODULUS)
		return 2;
	if (tok.type == EQUAL)
		return 2;
	if (tok.type == RETURN)
		return 1;
	if (tok.type == LESS || tok.type == GREATER || tok.type == LESS_OR_EQ || tok.type == GREATER_OR_EQ || tok.type == IS_EQUAL || tok.type == NOT_EQUAL)
		return 2;
	if (tok.type == VAR || tok.type == UNARY_NEG || tok.type == INC || tok.type == DEC || tok.type == LNOT)
		return 1;
	if (tok.type == LAND || tok.type == LNAND || tok.type == LNOR || tok.type == LOR)
		return 2;
	return 0;
}
