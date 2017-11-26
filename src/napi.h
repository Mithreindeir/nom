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

#ifndef NAPI_H
#define NAPI_H

#define _CRT_SECURE_NO_DEPRECATE 1

#include "cinterp.h"
#include "clexer.h"
#include "ast.h"
#include "ncomp.h"
#include "nstd.h"

//Opens and parses a file then runes
void nom_run_file(char * file);
//Function wrapper to be called in nom
void nom_import_wrapper(frame * cf);
//Imports a module and merges with current
void nom_import_file(frame * cf, char * file);
//Iterates through children to set var
void nom_set_var(frame * cf, nom_variable * n);
//Launches a nom interpreter / REPL
void nom_repl();
//Registers a C function to the interpreter
void nom_register_func(nom_interp * nom, char * name, nom_external_func func, int args);
//Registers a C funtion to a frame
void nom_register_func_frame(frame * f, char * name, nom_external_func func, int args);


#endif