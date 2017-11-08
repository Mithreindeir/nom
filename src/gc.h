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

#ifndef GARC_H
#define GARC_H

#include <stdlib.h>
#include <stdio.h>

typedef struct gc_el gc_el;

struct gc_el
{
	void * ptr;
	int ref;
	gc_el * next;
};

//Implements reference counting
typedef struct gc
{
	gc_el * el;
	int num_nodes;
} gc;

gc * gc_init();
void gc_destroy(gc * gc);
void gc_add(gc * gc, void * ptr);
void gc_free(gc * gc, void * ptr);
void gc_remove(gc * gc, void * ptr);
void gc_replace(gc * gc, void * nptr, void * optr);

#endif