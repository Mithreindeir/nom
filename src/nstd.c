#include "nstd.h"
#include <math.h>

void nom_print(frame * currentframe)
{
	if (currentframe->data_stack->stack_ptr > 0)
	{
		element e = currentframe->data_stack->elements[currentframe->data_stack->num_elements - 1];
		if (e.type == NUM) {
			nom_number n;
			store(currentframe->data_stack, &n, sizeof(nom_number), 0);
			if (floorf(n) == n)
				printf("%d", (int)n);
			else
				printf("%f", n);
		}
		else if (e.type == BOOL) {
			nom_boolean n;
			store(currentframe->data_stack, &n, sizeof(nom_boolean), 0);
			if (n)
				printf("True");
			else
				printf("False");
		}
		else if (e.type == STR)
		{
			nom_string str;
			store(currentframe->data_stack, &str, sizeof(nom_string), 0);
			printf("%s", str.str);
		}
	}
}

void nom_input(frame * currentframe)
{	
	char str[256];
	memset(str, 0, 255);
	fgets(str, 255, stdin);

	int s = strlen(str);
	char * mstr = malloc(s+1);
	memcpy(mstr, str, s);
	mstr[s] = '\0';
	int len = 0;
	int type = token_type(mstr, &len);

	if (type == INT) {
		push_number(currentframe->data_stack, atoi(mstr));
	}
	else if (type == FLOAT) {
		push_number(currentframe->data_stack, atof(mstr));
	}
	else {
		push_raw_string(currentframe->data_stack, mstr);
	}

}
