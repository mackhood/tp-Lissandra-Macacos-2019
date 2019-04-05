#include "Consola.h"

void consola()
{
	char* linea;
	linea = readline (">");
	while(1)
	{
		if(linea)
			add_history(linea);
		if(!strncmp(linea,"exit",4))
		{
			free(linea);
			break;
		}
		printf("%s\n", linea);
		free(linea);
		linea = malloc(sizeof(char*));
		linea = readline ("");
	}
}
