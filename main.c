#include <string.h>
#include <stdio.h>
#include <stdlib.h>

enum TOKEN_TYPE
{
	NUMBER,
	OPERATOR,
	LETTER,
	SYMBOL,
	EOF_S,
	UNKNOWN
};

int factor(char * s);
int term(char * s);
int expr(char * s);
//strtok with space delimiter and token ends when token type changed
char * strtok_type(char * s);
char * get_current_tok(char * s);

char operators[] = "+-*&/|%^()";
int num_operators = 8;
char symbols[] = ",.\[]{};:`~";
char string[] = "7 + 3 * (10 / (12 / (3 + 1) - 1)) / (2 + 3) - 5 - 3 + (8)";
int token_type(char c);

int main()
{
	char op;
	int lhs = 0, rhs = 0;
	int i = 0;
	int total = 0;
	int first = 1;
	int result = expr(string);
	printf("%s = %d", string, result);

	getch();
	return 0;
}
int cidx = 0;

int factor(char * s)
{
	//factor := integer
	char * tok = strtok_type(s);
	if (tok == NULL)
	{
			printf("SYNTAX ERROR AT: %s\n", tok);
			abort();
	}
	if (token_type(*tok) == NUMBER)
	{
		int num = atoi(tok);
		free(tok);
		return num;
	}
	else if (*tok == '(')
	{
		int ridx = cidx;

		for (int i = 1; *(tok + i) == '('; i++)
			ridx--;
		int nested = 1;
		int nested_size = 0;
		while (nested > 0)
		{
			char tc = s[ridx + nested_size];
			if (tc == '\0')
				break;
			if (tc == ')')
				nested--;
			if (tc == '(')
				nested++;
			
			if (nested == 0)
				break;
			nested_size++;
		}
		char * buf = malloc(nested_size+1);
		for (int i = 0; i <= nested_size; i++)
		{
			buf[i] = s[ridx + i];
		}
		buf[nested_size] = '\0';
		cidx = 0;
		int result = expr(buf);
		cidx = ridx + nested_size + 1;
		free(buf);
		return result;
	}
}
int term(char * s)
{
	//term := factor ((DIV | MUL)factor)
	int result = factor(s);
	while (1)
	{
		char * tok = get_current_tok(s);

		if (tok == NULL)
		{
			return result;
		}

		switch (*tok)
		{
		case '/':
			free(strtok_type(s));
			result /= factor(s);
			break;
		case '*':
			free(strtok_type(s));
			result *= factor(s);
			break;
		default:
			return result;
			break;
		}
		free(tok);
	}

	return result;
}

int expr(char * s)
{
	//expr := term ((DIV | MUL)term)
	int result = term(s);
	while (1)
	{
		char * tok = strtok_type(s);
		if (tok == NULL)
			return result;

		switch (*tok)
		{
		case '+':
			result += term(s);
			break;
		case '-':
			result -= term(s);
			break;
		}
		free(tok);

	}
	return result;
}

int token_type(char c)
{
	if (c >= 0x30 && c <= 0x39)
	{
		return NUMBER;
	}
	if ((c >= 0x61 && c <= 0x71) || (c >= 0x41 && c <= 0x5a))
	{
		return LETTER;
	}
	
	for (int i = 0; i < num_operators; i++)
	{
		if (c == operators[i])
			return OPERATOR;
	}
	if (c == 0)
	{
		return EOF_S;
	}

	return UNKNOWN;
}
//Gets current token without advancing over it
char * get_current_tok(char * s)
{
	if (!s || s[cidx] == '\0')
		return NULL;

	char * f = malloc(sizeof(char) * 100);
	int i = cidx, k = 0;
	while (s[i] == ' ')
		i++;

	int ctype = token_type(s[i]);

	while (s[i] != '\0')
	{
		if (s[i] == ' ' || token_type(s[i]) != ctype)
		{
			f[k] = 0;
			return f;
		}
		else {
			f[k] = s[i];
		}
		k++;
		i++;
	}
	f[k] = 0;
	return f;
}

char * strtok_type(char * s)
{
	if (!s || s[cidx] == '\0')
		return NULL;

	char * f = malloc(sizeof(char) * 100);
	int i = cidx, k = 0;
	while (s[i] == ' ')
		i++;

	int ctype = token_type(s[i]);
	while (s[i] != '\0')
	{
		if (s[i] == ' ' || token_type(s[i]) != ctype )
		{
			f[k] = 0;
			cidx = i;
			return f;
		}
		else {
			f[k] = s[i];
		}
		k++;
		i++;
	}
	cidx = i;
	f[k] = 0;
	return f;
}