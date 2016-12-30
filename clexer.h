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
	COMMA,
	PLUS,
	MINUS,
	DIVIDE,
	MULT,
	UNARY_NEG,
	BAND,
	BOR,
	BXOR,
	IDENTIFIER,
	NEWLINE
};

typedef struct token
{
	char * tok;
	int len;
	int type;
} token;

//Splits string into tokens
token * tokenize(char * file, int * num_tok);
//Returns a TOKEN_TYPE
int token_type(char * tok, int * len);
//Is this token an operator
int is_operator(token tok);
//Is this token a conditional 
int is_conditional(token tok);
//Returns number of operator precedence. 
int token_precedence(token tok);
//Is left associative or is it right associative
int token_associative(token tok);
//Returns number of operands (eg: binary, unary, etc)
int token_operands(token tok);

#endif