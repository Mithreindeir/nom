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

#include "redblack.h"

rb_node * rb_node_init()
{
	return NULL;
}

void rb_insert(rb_node * root, char * string)
{
	int v = lex_less(root->string, string);
	rb_node * child = NULL;
	int right = 1;
	if (v >= 0) {
		if (root->right) {
			rb_insert(root->right, string);
			return;
		}
		else {
			root->right = rb_new(string);
			root->right->parent = root;
			child = root->right;
		}
	}
	else {
		if (root->left) {
			rb_insert(root->left, string);
			return;
		}
		else {
			root->left = rb_new(string);
			root->left->parent = root;
			child = root->left;
			right = 0;
		}
	}
	if (root->rb)
	{
		if (right && (!root->left || (root->left && !root->left->rb)))
		{
			//rotate left
			rb_rotate_left(child);
		}
		else if (!right && (!root->right || (root->right && !root->right->rb)))
		{
			//rotate right
			rb_rotate_right(child);
		}
		else if (right && (!root->left || (root->left && root->left->rb)))
		{
			child->rb = 0;
			root->left->rb = 0;
			if (root->parent->parent != NULL) {
				root->parent->rb = 1;
			}
		}
		else if (!right && (!root->right || (root->right && root->right->rb)))
		{
			child->rb = 0;
			root->right->rb = 0;
			if (root->parent->parent != NULL) {
				root->parent->rb = 1;
			}
		}
	}
}

void rb_rotate_right(rb_node * node)
{
	rb_node * p = node->parent;
	p->left = node->right;
	node->rb = p;
}

void rb_rotate_left(rb_node * node)
{
	rb_node * p = node->parent;
	p->right = node->left;
	node->rb = p;
}

rb_node * rb_new(char * string)
{
	rb_node * rb = malloc(sizeof(rb_node));
	rb->left = NULL;
	rb->right = NULL;
	rb->parent = NULL;
	rb->string = string;
	rb->rb = 1;
	return rb;
}

void rb_delete(rb_node * root, char * string)
{
}

int lex_less(char * str1, char * str2)
{
	//return 1 if str1 < str2
	//Return -1 if str2 < str1
	//Return 0 if str1==str2

	int size1 = strlen(str1), size2 = strlen(str2);

	for (int i = 0; i < (size1 < size2 ? size1 : size2); i++) {
		if (str1[i] < str2[i])
			return 1;
		else if (str1[i] > str2[i])
			return -1;
	}

	// Letter compared to nothing, nothing is before letter
	if (size2 > size1)
		return 1;
	else if (size1 > size2)
		return -1;

	return 0;
}
