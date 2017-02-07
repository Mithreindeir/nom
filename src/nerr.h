#ifndef NERR_H
#define NERR_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//Functions to throw errors that may occur

void syntax_error(char * tok, int col, int row, char * reason);
void runtime_error(char * reason, int fatal);

#endif