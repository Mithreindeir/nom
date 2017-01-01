#include "nerr.h"

void syntax_error(char * tok, int col, int row, char * reason)
{
	printf("SYNTAX ERROR %s AT COL: %d ROW %d\n", tok, col, row);
	printf("%s\n", reason);
	getch();
	exit(0);
	abort();
}

void runtime_error(char * reason, int fatal)
{
	printf("ERROR: %s\n", reason);
	if (fatal)
	{
		printf("FATAL\n");
		getch();
		exit(0);
		abort();
	}
}
