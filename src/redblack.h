#ifndef REDBLACK_H
#define REDBLACK_H

#include <string.h>

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
void rb_delete(rb_node * root, char * string);

int lex_less(char * str1, char * str2);

#endif