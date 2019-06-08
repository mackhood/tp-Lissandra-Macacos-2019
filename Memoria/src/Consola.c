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
		{(char *) NULL, (Funcion *) NULL}
}; // para generalizar las funciones reciben un string.




void handleConsola(){


	puts("-_____________________________________________________");
	puts("CONSOLA");
	puts("------ Escriba un comando ------");
	puts("1. - SELECT  <tabla> <key>");
	puts("2. - INSERT <tabla> <key> <value>");
	puts("3. - CREATE <tabla>");
	puts("4. - DROP <tabla>");
	puts("5. - DESCRIBE <tabla> (puede dejarse vacia)");
	puts("4. - JOURNAL");
	char* linea;
//ejecutar prueba.txt
	while (1) {
		linea = readline("\nA continuacion puede solicitar sus request independientemente de las solicitudes de memoria:\n ");

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

	usleep(info_memoria.retardo_mp);

	free(nombre_tabla);
	free(value_buscado);
}

void insert(char** args){
	char* nombre_tabla = string_duplicate(args[1]);
	uint16_t key = atoi(args[2]);
	char* value = string_duplicate(args[3]);

	insertReq(nombre_tabla, key, value);

	usleep(info_memoria.retardo_mp);

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

	usleep(info_memoria.retardo_fs);
	usleep(info_memoria.retardo_mp);

	prot_destruir_mensaje(mensaje_fs);
	free(nombre_tabla);
	free(tipo_consistencia);
}

void drop(char** args){
	char* nombre_tabla = strdup(args[1]);

	t_prot_mensaje* respuesta_fs = dropReq(nombre_tabla);

	//posibles respuestas
	switch(respuesta_fs->head){
		case TABLE_DROP_OK:{
			printf("La tabla fue borrada\n");
		}break;
		case TABLE_DROP_NO_EXISTE:{
			printf("La tabla no existe\n");
		}break;
		case TABLE_DROP_FALLO:{
			printf("hubo un error\n");
		}break;
		default:{
			break;
		}
	}

	usleep(info_memoria.retardo_fs);
	usleep(info_memoria.retardo_mp);

	prot_destruir_mensaje(respuesta_fs);
	free(nombre_tabla);
}

void journal(char** args){

	printf("Se procedera a realizar el journal\n");
	pthread_mutex_lock(&mutex_estructuras_memoria);
	journalReq();
	pthread_mutex_unlock(&mutex_estructuras_memoria);

}

void describe(char** args){

	if(args[1] != NULL){
		char* nombre_tabla = strdup(args[1]);
		int largo_nombre_tabla = strlen(nombre_tabla);

		int tamanio_buffer = sizeof(int) + largo_nombre_tabla;
		void* buffer = malloc(tamanio_buffer);

		memcpy(buffer, &largo_nombre_tabla, sizeof(int));
		memcpy(buffer+sizeof(int), nombre_tabla, largo_nombre_tabla);

		prot_enviar_mensaje(socket_fs, DESCRIBE, tamanio_buffer, buffer);
		t_prot_mensaje* data_del_fs = prot_recibir_mensaje(socket_fs);

		if(data_del_fs->head == POINT_DESCRIBE){
			int largo_descripcion;
			char* descripcion_tabla;

			memcpy(&largo_descripcion, data_del_fs->payload, sizeof(int));
			descripcion_tabla = malloc(largo_descripcion+1);
			memcpy(descripcion_tabla, data_del_fs->payload+sizeof(int), largo_descripcion);
			descripcion_tabla[largo_descripcion]='\0';

			char* info_tabla = strtok(descripcion_tabla, ";");
			char** valores_separados = string_split(info_tabla, ",");

			printf("Nombre de la tabla:%s\n", valores_separados[0]);
			printf("Consistencia:%s\n", valores_separados[1]);
			printf("Cantidad de particiones:%s\n", valores_separados[2]);
			printf("Tiempo entre compactaciones:%s\n", valores_separados[3]);

			free(descripcion_tabla);
			free(info_tabla);
			liberadorDeArrays(valores_separados);
		}
		else if(data_del_fs->head == FAILED_DESCRIBE){
			printf("La tabla NO existe para realizar el describe\n");
		}

		free(buffer);
		prot_destruir_mensaje(data_del_fs);
		free(nombre_tabla);
	}
	else{
		prot_enviar_mensaje(socket_fs, DESCRIBE, 0, NULL);
		t_prot_mensaje* data_del_fs = prot_recibir_mensaje(socket_fs);

		if(data_del_fs->head == FULL_DESCRIBE){
			int largo_descripcion;
			char* descripcion_tabla;

			memcpy(&largo_descripcion, data_del_fs->payload, sizeof(int));
			descripcion_tabla = malloc(largo_descripcion+1);
			memcpy(descripcion_tabla, data_del_fs->payload+sizeof(int), largo_descripcion);
			descripcion_tabla[largo_descripcion]='\0';

			char** data_de_tablas_separadas = string_split(descripcion_tabla, ";");

			int i = 0;
			while(data_de_tablas_separadas[i]){
				char** valores_separados = string_split(data_de_tablas_separadas[i], ",");
				printf("Nombre de la tabla:%s\n", valores_separados[0]);
				printf("Consistencia:%s\n", valores_separados[1]);
				printf("Cantidad de particiones:%s\n", valores_separados[2]);
				printf("Tiempo entre compactaciones:%s\n", valores_separados[3]);

				liberadorDeArrays(valores_separados);
				i++;
			}

			free(descripcion_tabla);
			liberadorDeArrays(data_de_tablas_separadas);
		}
		else if(data_del_fs->head == FAILED_DESCRIBE){
			printf("El fs NO tiene ninguna tabla\n");
		}

		prot_destruir_mensaje(data_del_fs);
	}
}
