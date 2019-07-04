/*
 * Consola.c
 *
 *  Created on: 14 abr. 2019
 *      Author: utnso
 */

#include "Consola.h"


COMANDO comandos[] = {
		{"SELECT",selectt},
		{"INSERT",insert},
		{"CREATE",create},
		{"DESCRIBE",describe},
		{"DROP",drop},
		{"JOURNAL",journal},
		{(char *) NULL, (Funcion *) NULL}
}; // para generalizar las funciones reciben un string.




void handleConsola(){


	puts("-_____________________________________________________");
	puts("CONSOLA");
	puts("------ Escriba un comando ------");
	puts("1. - SELECT  <tabla> <key>");
	puts("2. - INSERT <tabla>|<key>|<value>");
	puts("3. - CREATE <tabla>");
	puts("4. - DROP <tabla>");
	puts("5. - DESCRIBE <tabla> (puede dejarse vacia)");
	puts("6. - JOURNAL");
	puts("\nAdvertencia: los pipes solo deben ser usados para separar los parámetros del Insert, en otras instrucciones"
			" no son considerados.");
	char* linea;
	//ejecutar prueba.txt
	while (1) {
		linea = readline("\nA continuacion puede solicitar sus request independientemente de las solicitudes de memoria:\n ");

		char* aux = malloc(strlen(linea) + 1);
		strcpy(aux, linea);
		string_to_lower(aux);
		if (!strcmp(aux, "exit"))
		{
			free(linea);
			puts("Esta memoria procederá a desconectarse.");
			clear_history();
			free(aux);
			break;
		}
		if(!strcmp(linea, ""))
			puts("Por favor, ingrese un comando.");
		else if (ejecutar_linea(linea))
		{
			add_history(linea);
		}
		free(aux);
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
	}
	else
	{
		funcion = string_duplicate(linea_aux);
	}


	COMANDO * comando = buscar_comando(funcion);

	if(!(comando != NULL))
	{
		puts("El comando que quiere ejecutar no coincide con ningún comando conocido.");
		free(funcion);
		free(linea_aux);
	}
	else if(doesNotContainNumber(comando->nombre))
	{
		char** args;
		if(!strcmp(comando->nombre, "INSERT"))
		{
			args = malloc(300);
			char* aux = strtok(linea_aux, " ");
			int count = 0;
			while(aux != NULL)
			{
				args[count] = aux;
				aux = strtok(NULL, "|");
				count++;
			}
			args[count] = aux;
		}
		else
			args = string_split(linea_aux, " ");
		if (!comando) {
			printf ("%s: el comando ingresado es incorrecto.\n", funcion);
			return (-1);
		}

		free(funcion);

		//Llamo a la función
		(*(comando->funcion)) (args);
		if(!strcmp(comando->nombre, "INSERT"))
		{
			free(args[0]);
			free(args);
		}
		else
		{
			free(linea_aux);
			liberadorDeArrays(args);
		}

	}
	else
	{
		puts("El comando que quiere ejecutar no tiene el formato especificado.");
		free(funcion);
		free(linea_aux);
	}
	return 1;
}

void selectt(char** args)
{
	log_info(loggerMem, "[Consola]: Se ha recibido un pedido de select.");
	if(chequearParametros(args, 3) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		log_error(loggerMem,"[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(!itsANumber(args[2]))
		{
			puts("La key que ingresó posee caracteres invalidos.");
		}
		else
		{
			char* tabla = malloc(strlen(args[1]) + 1);
			strcpy(tabla, args[1]);
			char* claveaux = malloc(strlen(args[2]) + 1);
			strcpy(claveaux, args[2]);
			uint16_t key = atoi(claveaux);
			char* value_buscado = selectReq(tabla, key);

			printf("el value buscado es %s\n", value_buscado);

			usleep(info_memoria.retardo_mp*1000);

			free(tabla);
			free(value_buscado);
		}
	}
}

void insert(char** args)
{
	log_info(loggerMem, "[Consola]: Se ha recibido un pedido de insert.");
	int chequeo = 0;
	chequeo = chequearParametros(args, 4);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		log_error(loggerMem, "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(!itsANumber(args[2]))
		{
			puts("La key que ingresó posee caracteres inválidos.");
			log_error(loggerMem, "[Consola]: la key a Insertar es inválida.");
		}
		else if(strlen(args[2]) > 5)
		{
			puts("La key que ingresó es demasiado grande.");
			log_error(loggerMem, "[Consola]: la key a Insertar es demasiado grande.");
		}
		else
		{
			char* tabla = malloc(strlen(args[1]) + 1);
			strcpy(tabla, args[1]);
			char* claveaux = malloc(strlen(args[2]) + 1);
			strcpy(claveaux, args[2]);
			if(atoi(claveaux) > 65536)
			{
				puts("La key es demasiado grande, ingrese una más pequeña.");
				log_error(loggerMem, "[Consola]: la key a Insertar es inválida.");
				free(tabla);
				free(claveaux);
				return;
			}
			uint16_t key = atoi(claveaux);
			if(string_contains(args[3], ";"))
			{
				printf("Por favor, ingrese un value que no tenga ';' dentro.");
				log_error(loggerMem,"[Consola]: se ingresó un value inválido");
			}
			else
			{
				char* value = malloc(strlen(args[3]) + 1);
				strcpy(value, args[3]);
				insertReq(tabla, key, value);
				free(tabla);
				free(value);
			}
		}
	}
	usleep(info_memoria.retardo_mp*1000);
}

void create(char** args)
{
	log_info(loggerMem, "[Consola]: Se ha recibido un pedido de create.");
	if(chequearParametros(args, 5) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		log_error(loggerMem, "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(criterioInvalido(args[2]))
		{
			puts("El criterio es inválido.");
			log_error(loggerMem, "[Consola]: el criterio de CREATE es inválido.");
		}
		else if(!itsANumber(args[3]))
		{
			puts("La cantidad de particiones es inválida.");
			log_error(loggerMem, "[Consola]: la cantidad de particiones para CREATE es inválida.");
		}
		else if(atoi(args[3]) > 0)
		{
			puts("La cantidad de particiones es inválida.");
			log_error(loggerMem, "[Consola]: la cantidad de particiones para CREATE es inválida.");
		}
		else if(!itsANumber(args[4]))
		{
			puts("El tiempo entre cada compactación es inválido.");
			log_error(loggerMem, "[Consola]: el tiempo entre cada compactación para CREATE es inválido.");
		}
		else
		{
			char* nombre_tabla = strdup(args[1]);
			int largo_nombre_tabla = strlen(nombre_tabla);
			char* tipo_consistencia = strdup(args[2]);
			int largo_tipo_consistencia = strlen(tipo_consistencia);
			int numero_particiones = atoi(args[3]);
			int compaction_time = atoi(args[4]);

			t_prot_mensaje* mensaje_fs = createReq(nombre_tabla, largo_nombre_tabla, tipo_consistencia, largo_tipo_consistencia, numero_particiones, compaction_time);
			switch(mensaje_fs->head)
			{
			case TABLA_CREADA_OK:
			{
				printf("la tabla fue creada\n");
			}break;
			case TABLA_CREADA_YA_EXISTENTE:
			{
				printf("la tabla ya se encuentra existente\n");
			}break;
			case TABLA_CREADA_FALLO:
			{
				printf("hubo un error al crear la tabla\n");
			}break;
			default:
			{
				break;
			}
			}

			usleep(info_memoria.retardo_fs*1000);
			usleep(info_memoria.retardo_mp*1000);

			prot_destruir_mensaje(mensaje_fs);
			free(nombre_tabla);
			free(tipo_consistencia);
		}
	}
}

void drop(char** args)
{
	log_info(loggerMem, "[Consola]: Se ha solicitado realizar un DROP");
	if(chequearParametros(args, 2) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada\n");
		log_error(loggerMem, "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* nombre_tabla = strdup(args[1]);

		t_prot_mensaje* respuesta_fs = dropReq(nombre_tabla);

		//posibles respuestas
		switch(respuesta_fs->head)
		{
		case TABLE_DROP_OK:
		{
			printf("La tabla fue borrada\n");
		}break;
		case TABLE_DROP_NO_EXISTE:
		{
			printf("La tabla no existe\n");
		}break;
		case TABLE_DROP_FALLO:
		{
			printf("hubo un error\n");
		}break;
		default:
		{
			break;
		}
		}

		usleep(info_memoria.retardo_fs*1000);
		usleep(info_memoria.retardo_mp*1000);

		prot_destruir_mensaje(respuesta_fs);
		free(nombre_tabla);
	}
}

void journal(char** args)
{
	log_info(loggerMem, "[Consola]: Se ha recibido un pedido de describe.");
	int chequeo = 0;
	chequeo = chequearParametros(args, 1);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		log_error(loggerMem, "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		printf("Se procedera a realizar el journal\n");
		pthread_mutex_lock(&mutex_estructuras_memoria);
		journalReq();
		pthread_mutex_unlock(&mutex_estructuras_memoria);

		usleep(info_memoria.retardo_mp*1000);
	}
}

void describe(char** args)
{

	log_info(loggerMem, "[Consola]: Se ha recibido un pedido de describe.");
	int chequeo = 0;
	if(args[1] == NULL )
		chequeo = chequearParametros(args, 1);
	else
		chequeo = chequearParametros(args, 2);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		log_error(loggerMem, "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(args[1] != NULL)
		{
			char* nombre_tabla = strdup(args[1]);
			int largo_nombre_tabla = strlen(nombre_tabla);

			int tamanio_buffer = sizeof(int) + largo_nombre_tabla;
			void* buffer = malloc(tamanio_buffer);

			memcpy(buffer, &largo_nombre_tabla, sizeof(int));
			memcpy(buffer+sizeof(int), nombre_tabla, largo_nombre_tabla);

			prot_enviar_mensaje(socket_fs, DESCRIBE, tamanio_buffer, buffer);
			t_prot_mensaje* data_del_fs = prot_recibir_mensaje(socket_fs);

			if(data_del_fs->head == POINT_DESCRIBE)
			{
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
		else
		{
			prot_enviar_mensaje(socket_fs, DESCRIBE, 0, NULL);
			t_prot_mensaje* data_del_fs = prot_recibir_mensaje(socket_fs);

			if(data_del_fs->head == FULL_DESCRIBE)
			{
				int largo_descripcion;
				char* descripcion_tabla;

				memcpy(&largo_descripcion, data_del_fs->payload, sizeof(int));
				descripcion_tabla = malloc(largo_descripcion+1);
				memcpy(descripcion_tabla, data_del_fs->payload+sizeof(int), largo_descripcion);
				descripcion_tabla[largo_descripcion]='\0';

				char** data_de_tablas_separadas = string_split(descripcion_tabla, ";");

				int i = 0;
				while(data_de_tablas_separadas[i])
				{
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
			else if(data_del_fs->head == FAILED_DESCRIBE)
			{
				printf("El fs NO tiene ninguna tabla\n");
			}

			prot_destruir_mensaje(data_del_fs);
		}
	}
}
