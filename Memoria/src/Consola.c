/*
 * Consola.c
 *
 *  Created on: 14 abr. 2019
 *      Author: utnso
 */

#include "Consola.h"


COMANDO comandos[] = {
		{"select",selectt},
		{"insert",insert},
		{"create",create},
		{"DESCRIBE",describe},
		{"drop",drop},
		{"journal",journal},
		//{(char *) NULL, (Function *) NULL}
}; // para generalizar las funciones reciben un string.




void handleConsola(){


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

	if(string_contains(linea, "|"))
	{
			while (linea_aux[i] != '|') i++;

			funcion = malloc((sizeof(char) * i) + 1);
			strncpy(funcion, linea_aux, i);
			funcion[i] = '\0';
	}
	else
	{
		if(string_contains(linea, " "))
		{
			while (linea_aux[i] != ' ') i++;

			funcion = malloc((sizeof(char) * i) + 1);
			strncpy(funcion, linea_aux, i);
			funcion[i] = '\0';
		}
		else
		{
			funcion = string_duplicate(linea_aux);
		}
	}

	COMANDO * comando = buscar_comando(funcion);

	char** args;
	if(!strcmp(comando->nombre, "INSERT"))
	{
		args = string_split(linea_aux, "|");
	}
	else
		args = string_split(linea_aux, " ");
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

void selectt(char** args){
	char* nombre_tabla = string_duplicate(args[1]);
	uint16_t key = atoi(args[2]);

	char* value_buscado = selectReq(nombre_tabla, key);

	printf("el value buscado es %s\n", value_buscado);
	free(nombre_tabla);
	free(value_buscado);
}

void insert(char** args){
	char* nombre_tabla = string_duplicate(args[1]);
	uint16_t key = atoi(args[2]);
	char* value = string_duplicate(args[3]);

	double time_actualizado = insertReq(nombre_tabla, key, value);

	printf("el nuevo time para la key %d es %f\n", key, time_actualizado);
	free(nombre_tabla);
	free(value);
}

void create(char** args){
	char* nombre_tabla = strdup(args[1]);
	int largo_nombre_tabla = strlen(nombre_tabla);
	char* tipo_consistencia = strdup(args[2]);
	int largo_tipo_consistencia = strlen(tipo_consistencia);
	int numero_particiones = atoi(args[3]);
	int compaction_time = atoi(args[4]);

	t_prot_mensaje* mensaje_fs = createReq(nombre_tabla, largo_nombre_tabla, tipo_consistencia, largo_tipo_consistencia, numero_particiones, compaction_time);
	switch(mensaje_fs->head){
			case TABLA_CREADA_OK:{
				printf("la tabla fue creada\n");
			}break;
			case TABLA_CREADA_YA_EXISTENTE:{
				printf("la tabla ya se encuentra existente\n");
			}break;
			case TABLA_CREADA_FALLO:{
				printf("hubo un error al crear la tabla\n");
			}break;
			default:{
				break;
			}
		}

	prot_destruir_mensaje(mensaje_fs);
	free(nombre_tabla);
	free(tipo_consistencia);
}
