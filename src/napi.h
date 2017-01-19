#ifndef NAPI_H
#define NAPI_H

#define _CRT_SECURE_NO_DEPRECATE 1

#include "cinterp.h"
#include "clexer.h"
#include "ast.h"
#include "ncomp.h"
#include "nstd.h"

void nom_run_file(char * file);
void nom_repl();
void nom_register_func(nom_interp * nom, char * name, nom_external_func func);


#endif