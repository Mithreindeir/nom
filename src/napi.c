#include "napi.h"

void nom_run_file(char * file)
{
	nom_interp * nom;
	char * buffer = 0;
	long length;
	FILE * f = fopen(file, "r+");
	if (f)
	{
		fseek(f, 0, SEEK_END);
		length = ftell(f);
		fseek(f, 0, SEEK_SET);
		buffer = malloc(length + 1);
		memset(buffer, 0, length + 1);
		if (buffer)
		{
			fread(buffer, 1, length, f);
		}
		fclose(f);
		buffer[length] = '\0';
	}
	if (buffer)
	{
		nom = nom_interp_init();
		int num_tokens = 0;
		token * tokens = tokenize(buffer, &num_tokens);
		node * bop = parse_string(tokens, num_tokens);
		compile(bop, nom->global_frame);
		//getch();
		for (int i = 0; i < num_tokens; i++)
		{
			//printf("token: %s\t type: %d\n", tokens[i].tok, tokens[i].type);
			free(tokens[i].tok);
		}
		free(tokens);
		//return;
		stack_init(&nom->global_frame->data_stack);
		execute(nom->global_frame);
		free(buffer);
		nom_interp_destroy(nom);
	}
}

void nom_repl()
{
	// Don't use this 
	nom_interp * nom;
	nom = nom_interp_init();

	printf(">");
	char buff[256];
	memset(buff, 0, 255);
	fgets(buff, 255, stdin);
	int scompile = 1;
	while (strncmp(buff, "exit", 4))
	{
		int blocks = 0;
		int num_tokens = 0;
		//printf("%s\n", buff);
		//getch();
		token * tokens = tokenize(buff, &num_tokens);
		for (int i = 0; i < num_tokens; i++)
		{
			token tok = tokens[i];
			if (tok.type == COLON) {
				blocks++;
			}
			if (tok.type == END) {
				blocks--;
			}
		}
		if (blocks == 0)
			scompile = 1;
		else
			scompile = 0;

		if (scompile)
		{

			node * bop = parse_string(tokens, num_tokens);
			compile(bop, nom->global_frame);

			//getch();
			for (int i = 0; i < num_tokens; i++)
			{
				//printf("token: %s\t type: %d\n", tokens[i].tok, tokens[i].type);
				free(tokens[i].tok);
			}
			free(tokens);
			//return;
			stack_init(&nom->global_frame->data_stack);
			execute(nom->global_frame);
		}

		if (scompile)
		{
			printf("\n>");
			memset(buff, 0, 255);
			fgets(buff, 255, stdin);
		}
		else
		{
			int size = strlen(buff);
			fgets(buff + size, 255 - size, stdin);
		}
	}
	nom_interp_destroy(nom);

}
