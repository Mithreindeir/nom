#ifndef CLEXER_H
#define CLEXER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum TOKEN_TYPES
{
	IF,
	ELSE,
	FOR,
	END,
	WHILE,
	FUNCTION,
	EQUAL,
	IS_EQUAL,
	NOT_EQUAL,
	GREATER,
	GREATER_OR_EQ,
	LESS,
	LESS_OR_EQ,
	RPAREN,
	LPAREN,
	RBRACK,
	LBRACK,
	RCBRACK,
	LCBRACK,
	QUOTATION,
	FLOAT,
	INT,
	STRING,
	COLON,
	PLUS,
	MINUS,
	DIVIDE,
	MULT,
	BAND,
	BOR,
	BXOR,
	IDENTIFIER
};

typedef struct token
{
	char * tok;
	int len;
	int type;
} token;

token * tokenize(char * file, int * num_tok);
int token_type(char * tok, int * len);
int is_operator(token tok);
int token_precedence(token tok);

#endif