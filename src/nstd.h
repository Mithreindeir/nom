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

#ifndef NSTD_H
#define NSTD_H

#include <stdio.h>
#include "cinterp.h"
#include "clexer.h"
#include "ast.h"
#include "ncomp.h"


//Functions in the nom standard library
void nom_print(frame * currentframe);
void nom_input(frame * currentframe);
void nom_seed(frame * currentframe);
void nom_random(frame * currentframe);
void nom_time(frame * currentframe);
void nom_clock(frame * currentframe);
void nom_abs(frame * currentframe);
void nom_floor(frame * currentframe);
void nom_ceil(frame * currentframe);
void nom_run(frame * currentframe);

void nom_string_init(frame * currentframe);
void nom_size(frame * currentframe);
void nom_reserve(frame * currentframe);

void nom_open(frame * currentframe);
void nom_close(frame * currentframe);
void nom_write(frame * currentframe);
void nom_read(frame * currentframe);
void nom_readline(frame * currentframe);

#endif