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
	gcol->last_el = NULL;
	gcol->lfreed = NULL;
	gcol->ffreed = NULL;
	gcol->num_nodes = 0;
	gcol->cfree = 0;

	return gcol;
}

void gc_destroy(gc * gcol)
{
	gc_el * cur = gcol->ffreed;
	while (cur) {
		gc_el * n = cur->next;
		free(cur);
		cur = n;
	}

	if (gcol->num_nodes > 0) {
		cur = gcol->last_el;
		while (cur) {
			gc_el * n = cur->last;
			free(cur->ptr);
			free(cur);
			cur = n;
		}
	}
	//printf("Average Iter %d\n Average N %d\n", gcol->efficiency/gcol->samples, gcol->av_n/gcol->samples);
	free(gcol);
}

void gc_add(gc * gcol, void * ptr)
{
	if (gcol->num_nodes > 0) {
		gc_el * cur = gcol->last_el;
		int iter = 0;
		while (cur != NULL) {
			iter++;
			if (cur->ptr == ptr) {
				cur->ref++;
				return;
			}
			cur = cur->last;
		}
		//gcol->efficiency += iter;
		//gcol->av_n += gcol->num_nodes;
		//gcol->samples++;
		gc_el * gcel;
		gcel = malloc(sizeof(gc_el));
		gcel->ptr = ptr;
		gcel->ref = 1;
		gcel->next = NULL;
		gcol->num_nodes++;
		gcol->last_el->next = gcel;
		gcel->last = gcol->last_el;
		gcol->last_el = gcel;
	} else {
		gc_el * gcel;
		gcel = malloc(sizeof(gc_el));
		gcel->ptr = ptr;
		gcel->ref = 1;
		gcel->next = NULL;
		gcol->el = gcel;
		gcol->num_nodes = 1;
		gcel->last = NULL;
		gcol->last_el = gcel;
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
	gc_el * remove = NULL;
	while (cur) {
		if (cur->ptr == ptr) {
			if (cur == gc->el) {
				gc->el = cur->next;
			} else last->next = cur->next;
			remove = cur;
			break;
		}
		if (cur != gc->el) last = last->next;
		cur = cur->next;
	}
	if (remove) {
		remove->next = NULL;
		remove->last = NULL;
		if (gc->lfreed) {	
			gc->lfreed->next = remove;
			remove->last = gc->lfreed;
			gc->lfreed = gc->lfreed->next;
		} else {
			gc->ffreed = remove;
			gc->lfreed = remove;
		}
	}
}

void gc_free(gc * gc, void * ptr)
{
	gc_el * cur = gc->last_el;
	gc_el * remove = NULL;
	int f = 0;
	if (cur) {
		if (cur->ptr == ptr) {
			cur->ref--;
			if (cur->ref <= 0) {
				if (cur->last) {
					gc->last_el = cur->last;
					gc->last_el->next = NULL;
				} else {
					gc->el = NULL;
					gc->last_el = NULL;
				}
				gc->num_nodes--;
				free(cur->ptr);
				cur->ptr = NULL;
				remove = cur;
				gc->cfree++;
				ptr = NULL;
				free(cur);
				return;
			}
			f = 1;
		}
	}
	//int iter = 1;
	if (!f) {
		cur = cur->last;
		while (cur) {
			//iter++;	
			if (cur->ptr == ptr) {
				cur->ref--;
				if (cur->ref <= 0) {
					if (cur->last) {
						cur->last->next = cur->next;
						cur->next->last = cur->last;
					} else {
						cur->next->last = NULL;
						gc->el = cur->next;
					}
					gc->num_nodes--;
					free(cur->ptr);
					cur->ptr = NULL;
					remove = cur;
					free(cur);
					ptr = NULL;

				}
				return;
			}
			cur = cur->last;
		}
	}
	return;
	//gc->efficiency += iter;
	//gc->av_n += gc->num_nodes;
	//gc->samples++;
	if (remove) {
		remove->next = NULL;
		remove->last = NULL;
		if (gc->lfreed) {
			gc->lfreed->next = remove;
			remove->last = gc->lfreed;
			gc->lfreed = gc->lfreed->next;
			gc->lfreed->next = NULL;
		} else {
			gc->ffreed = remove;
			gc->lfreed = remove;
			gc->lfreed->last = NULL;
			gc->ffreed->last = NULL;
		}
	}
}