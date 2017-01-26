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

#ifndef REDBLACK_H
#define REDBLACK_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct rb_node rb_node;

struct rb_node
{
	char * string;
	rb_node * left;
	rb_node * right;
	//1 if red, 0 if black
	int rb;
};

rb_node * rb_node_init();
void rb_insert(rb_node * root, char * string);
rb_node * rb_new(char * string);
void rb_delete(rb_node * root, char * string);

int lex_less(char * str1, char * str2);

#endif
