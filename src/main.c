#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "napi.h"


int main(int argc, char ** argv)
{
	if (argc < 2) {
		printf("usage: %s filename\n", argv[0]);
		return 1;
	}

	nom_run_file(argv[1]);

	return 0;
}
