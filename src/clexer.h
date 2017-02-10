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

#ifndef CLEXER_H
#define CLEXER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "nerr.h"

//Types of recognizable tokens
enum TOKEN_TYPES
{
	IF,
	IFELSE,
	ELSE,
	ELSEIF,
	ELSEIFELSE,
	FOR,
	END,
	WHILE,
	RETURN,
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
	INC,
	DEC,
	UNARY_NEG,
	BAND,
	BOR,
	BXOR,
	LAND,
	LNOT,
	LNAND,
	LOR,
	LNOR,
	IDENTIFIER,
	NEWLINE,
	COMMENT,
	FUNC_CALL
};

typedef struct token
{
	char * tok;
	int len;
	int type;
	//For Errors
	int col, row;
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
//Returns number of idxs (eg: binary, unary, etc)
int token_idxs(token tok);

#endif