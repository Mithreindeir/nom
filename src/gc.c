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
#include "gc.h"


gc * gc_init()
{
	gc * gcol = malloc(sizeof(gc));
	gcol->el = NULL;
	gcol->num_nodes = 0;
	return gcol;
}

void gc_destroy(gc * gcol)
{
	if (gcol->num_nodes > 0) {
		gc_el * cur = gcol->el;
		while (cur) {
			gc_el * n = cur->next;
			free(cur->ptr);
			free(cur);
			cur = n;
		}
	}
	free(gcol);
}

void gc_add(gc * gcol, void * ptr)
{
	if (gcol->num_nodes > 0) {
		gc_el * cur = gcol->el;
		gc_el * last = NULL;
		while (cur != NULL) {
			if (cur->ptr == ptr) {
				cur->ref++;
				return;
			}
			if (cur->next == NULL) last = cur;
			cur = cur->next;
		}
		if (last) {
			gc_el * gcel = malloc(sizeof(gc_el));
			gcel->ptr = ptr;
			gcel->ref = 1;
			gcel->next = 0;
			gcol->num_nodes++;
			last->next = gcel;
		}
	} else {
		gc_el * gcel = malloc(sizeof(gc_el));
		gcel->ptr = ptr;
		gcel->ref = 1;
		gcel->next = 0;
		gcol->el = gcel;
		gcol->num_nodes = 1;
	}
}

void gc_replace(gc * gc, void * nptr, void * optr)
{
	gc_el * cur = gc->el;
	gc_el * last = gc->el;
	while (cur) {
		if (cur->ptr == optr) {
			cur->ptr = nptr;
			return;
		}
		if (cur != gc->el) last = last->next;
		cur = cur->next;
	}
}

void gc_remove(gc * gc, void * ptr)
{
	gc_el * cur = gc->el;
	gc_el * last = gc->el;
	while (cur) {
		if (cur->ptr == ptr) {
			if (cur == gc->el) {
				gc->el = cur->next;
			} else last->next = cur->next;
			free(cur);
			return;
		}
		if (cur != gc->el) last = last->next;
		cur = cur->next;
	}
}

void gc_free(gc * gc, void * ptr)
{
	gc_el * cur = gc->el;
	gc_el * last = gc->el;
	while (cur) {
		if (cur->ptr == ptr) {
			cur->ref--;
			if (cur->ref <= 0) {
				if (cur == gc->el) {
					gc->el = cur->next;
				} else last->next = cur->next;
				free(cur);
				free(ptr);
				ptr = NULL;
			}
			return;
		}
		if (cur != gc->el) last = last->next;
		cur = cur->next;
	}
}