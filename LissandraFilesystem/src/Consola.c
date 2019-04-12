#include "Consola.h"

COMANDO comandos[] = {
		{"select",selectt},
		{"insert",insert},
		{"create",create},
		{"DESCRIBE",describe},
		{"drop",drop},
		{(char *) NULL, (Function *) NULL}
};

void consola()
{
	puts("-_____________________________________________________");
	puts("CONSOLA");
	puts("------ Escriba un comando ------");
	puts("1. - Ejecutar  <ruta-al-escritorio>");
	puts("2. - Status    <id_dtb>");
	puts("3. - Finalizar <id_dtb>");
	puts("4. - Metricas  <id_dtb>");
	char* linea;
//ejecutar prueba.txt
	while (1) {
		linea = readline("\nSharks: ");

		if (strcmp(linea, "exit")==0){
			free(linea);
			puts("EXIT.");
			break;
		}

		if (ejecutar_linea(linea)){
			add_history(linea);
		}
		free(linea);
	}
}

COMANDO * buscar_comando(char* linea) {
	int i;

	for (i = 0; comandos[i].nombre; i++){
		// Paso a miniscula y comparo
		string_to_lower(linea);
		char* comando = string_duplicate(comandos[i].nombre);
		string_to_lower(comando);

		if (strcmp (linea, comando) == 0){
			free(comando);
			return (&comandos[i]);
		}
		free(comando);

	}
	return ((COMANDO *) NULL);
}

int ejecutar_linea (char * linea){
	char * linea_aux = string_duplicate(linea);
	int i = 0;
	char * funcion;

	if(string_contains(linea, " "))
	{
			while (linea_aux[i] != ' ') i++;

			funcion = malloc((sizeof(char) * i) + 1);
			strncpy(funcion, linea_aux, i);
			funcion[i] = '\0';
	}else{
			funcion = string_duplicate(linea_aux);
	}

	COMANDO * comando = buscar_comando(funcion);

	char** args = string_split(linea_aux, " ");
	if (!comando) {
		printf ("%s: el comando ingresado es incorrecto.", funcion);
		return (-1);
	}

	free(funcion);
	free(linea_aux);
	//Llamo a la función
	(*(comando->funcion)) (args);
	free(args[0]);
	free(args[1]);

	free(args);
	return 1;
}

void selectt (char** args)
{

}

void insert (char** args)
{

}
void create (char** args)
{

}
void describe (char** args)
{

}
void drop (char** args)
{

}
