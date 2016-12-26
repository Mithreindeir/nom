#include "clexer.h"
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS 1
#endif
int tcidx = 0;

char * strtokm(char * s)
{
	if (!s || s[tcidx] == '\0')
		return NULL;

	char * f = malloc(sizeof(char) * 100);
	int i = tcidx, k = 0;
	while (s[i] == ' ')
		i++;

	while (s[i] != '\0')
	{
		if (s[i] == ' ')
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
token * tokenize(char * file, int * num_tok)
{
	char * tok = strtokm(file, ' ');
	int num_tokens = 0;
	token * tokens = NULL;
	int current_idx = 0;
	while (tok != NULL)
	{
		num_tokens++;
		if (num_tokens == 1)
		{
			tokens = malloc(sizeof(token) * num_tokens);
		}
		else
		{
			tokens = realloc(tokens, sizeof(token) * num_tokens);
		}

		int len = 0;
		int type = token_type(tok, &len);
		if (len == 0)
			continue;
		int tlen = strlen(tok);
		//printf("token: %s\t type: %d\n", str, type);

		token ctoken;
		ctoken.tok = malloc(len);
		ctoken.len = len;
		ctoken.type = type;
		int i = 0;
		while (i < len)
		{
			ctoken.tok[i] = tok[i];
			i++;
		}
		ctoken.tok[i] = '\0';
		tokens[num_tokens-1] = ctoken;

		if (tlen <= len) {
			free((tok - current_idx));
			tok = strtokm(file, ' ');
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


int token_type(char * tok, int * len)
{
	*len = 0;
	if (!strncmp(tok, "if", 2))
	{
		*len = 2;
		return IF;
	}
	else if (!strncmp(tok, "else", 4))
	{
		*len = 4;
		return ELSE;
	}
	else if (!strncmp(tok, "end",3))
	{
		*len = 3;
		return END;
	}
	else if (!strncmp(tok, "for", 3))
	{
		*len = 3;
		return FOR;
	}
	else if (!strncmp(tok, "while", 5))
	{
		*len = 5;
		return WHILE;
	}
	else if (!strncmp(tok, "function", 8))
	{
		*len = 8;
		return FUNCTION;
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
	else if (!strncmp(tok, "\"", 1))
	{
		*len = 1;
		return QUOTATION;
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
	else if (!strncmp(tok, "+", 1))
	{
		*len = 1;
		return PLUS;
	}
	else if (!strncmp(tok, "-", 1))
	{
		*len = 1;
		return MINUS;
	}
	else if (!strncmp(tok, "*", 1))
	{
		*len = 1;
		return MULT;
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

	return 0;
}

int is_operator(token tok)
{
	if (tok.type == PLUS || tok.type == MINUS || tok.type == DIVIDE || tok.type == MULT)
		return 1;
	return 0;
}

int token_precedence(token tok)
{
	if (tok.type == PLUS || tok.type == MINUS)
		return 1;
	if (tok.type == MULT || tok.type == DIVIDE)
		return 2;
	return 0;
}
