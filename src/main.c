#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "napi.h"


int main()
{

	nom_run_file("tests/factorial.nom");
	getch();

	return;
}
