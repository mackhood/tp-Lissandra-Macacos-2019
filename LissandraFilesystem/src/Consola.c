#include "Consola.h"

COMANDO comandos[] = {
		{"SELECT",selectt},
		{"INSERT",insert},
		{"CREATE",create},
		{"DESCRIBE",describe},
		{"DROP",drop},
		{(char *) NULL, (Funcion *) NULL}
};

void consola()
{
	printf("\033[1;33m");
	puts("°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°");
	puts("----------------------------------------Lissandra FileSystem--------------------------------------------");
	puts("---------------------------------------- Escriba un comando --------------------------------------------");
	printf("\033[0;33m");
	puts("1. - SELECT <Table> <Key>");
	puts("2. - INSERT|<Table>|<Key>|<Value>|<Timestamp> (last one optional)");
	puts("3. - CREATE <Table> <Consistency> <Amount of partitions> <Time until next compaction>");
	puts("4. - DESCRIBE <Table> (Table is optional, if you want all tables to be shown, leave this parameter empty");
	puts("5. - DROP <Table>");
	printf("\033[1;31m");
	puts("Detalle amistoso: Para los insert usar pipes o '|' no usar pipes en ninguna otra función ni nombre por consola, gracias.");
	char* linea;
//ejecutar prueba.txt
	while (1) {
		printf("\033[1;36m");
		linea = readline("\nIngrese el comando a ejecutar con los parametros necesarios:\n ");

		if (strcmp(linea, "exit")==0){
			free(linea);
			puts("EXIT.");
			printf("\033[0m");
			clear_history();
			pthread_mutex_unlock(&deathProtocol);
			break;
		}
		printf("\033[1;34m");
		if(!strcmp(linea, ""))
			continue;
		else if (ejecutar_linea(linea)){
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
	liberadorDeArrays(args);
	usleep(retardo * 1000);
	return 1;
}

void selectt (char** args)
{
	logInfo( "Consola: Se ha recibido un pedido de select.");
	if(chequearParametros(args, 3) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "Consola: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* tabla = malloc(strlen(args[1]) + 1);
		strcpy(tabla, args[1]);
		char* claveaux = malloc(strlen(args[2]) + 1);
		strcpy(claveaux, args[2]);
		uint16_t key = atoi(claveaux);
		t_keysetter* keysetterObtenido = selectKey(tabla, key);
		if(keysetterObtenido != NULL)
			printf("La clave obtenida mas actualizada es %i,%lf,%s\n", keysetterObtenido->key, keysetterObtenido->timestamp, keysetterObtenido->clave);
		else
			printf("La tabla que usted quiso acceder, o la clave que usted buscó, no existía dentro del File System.\n");
		free(tabla);
		free(claveaux);
	}
}

void insert (char** args)
{
	logInfo( "Consola: Se ha recibido un pedido de insert.");
	int chequeo = 0;
	if(args[4] == NULL )
		chequeo = chequearParametros(args, 4);
	else
		chequeo = chequearParametros(args, 5);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "Consola: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* tabla = malloc(strlen(args[1]) + 1);
		strcpy(tabla, args[1]);
		char* claveaux = malloc(strlen(args[2]) + 1);
		strcpy(claveaux, args[2]);
		uint16_t key = atoi(claveaux);
		if(string_contains(args[3], ";"))
		{
			printf("Por favor, ingrese un value que no tenga ';' dentro.");
			logError("Consola: se ingreso un value invalido");
		}
		else
		{
			char* value = malloc(strlen(args[3]) + 1);
			strcpy(value, args[3]);
			int result = 0;
			if(args[4] == NULL)
			{
				double timestampact = getCurrentTime();
				printf("Current time: %lf\n", timestampact);
				result = insertKeysetter(tabla, key, value, timestampact);
			}
			else
			{
				char* timestampaux = malloc(strlen(args[4]) + 1);
				strcpy(timestampaux, args[4]);
				double timestamp = atoi(timestampaux);
				result = insertKeysetter(tabla, key, value, timestamp);
			}
			switch(result)
			{
			case 0:
			{
				logInfo( "Consola: Insert realizado.");
				break;
			}
			case 1:
			{
				logError("Consola: Insert fallido por tabla inexistente");
				break;
			}
			case 2:
			{
				logError("Consola: la memtable no pudo recibir la clave");
				break;
			}
			case 3:
			{
				logError("Consola: el value era demasiado grande para realizar el insert");
				break;
			}
			}
		}
	}
}

void create (char** args)
{
	logInfo( "Consola: Se ha recibido un pedido de create.");
	if(chequearParametros(args, 5) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "Consola: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* tabla = malloc(strlen(args[1]) + 1);
		strcpy(tabla, args[1]);
		char* consistencia = malloc(strlen(args[2]) + 1);
		strcpy(consistencia, args[2]);
		char* particionesaux = malloc(strlen(args[3]) + 1);
		strcpy(particionesaux, args[3]);
		int particiones = atoi(particionesaux);
		char* intervaloCompactacionaux = malloc(strlen(args[4]) + 1);
		strcpy(intervaloCompactacionaux, args[4]);
		int intervaloCompactacion = atoi(intervaloCompactacionaux);
		switch(llamadoACrearTabla(tabla, consistencia, particiones, intervaloCompactacion))
		{
			case 0:
			{
				printf("Operación exitosa\n");
				logInfo( "Consola: Tabla creada satisfactoriamente");
				break;
			}
			case 2:
			{
				printf("La tabla solicitada ya existe\n");
				logInfo( "Consola: Tabla ya existía");
				break;
			}
			default:
			{
				printf("Error al crear la tabla\n");
				logError( "Consola: La tabla o alguna de sus partes no pudo ser creada");
				break;
			}
		}
		free(consistencia);
		free(particionesaux);
		free(intervaloCompactacionaux);
	}

}

void describe (char** args)
{
	logInfo( "Consola: Se ha recibido un pedido de describe.");
	int chequeo = 0;
	if(args[1] == NULL )
		chequeo = chequearParametros(args, 1);
	else
		chequeo = chequearParametros(args, 2);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "Consola: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* tablaSolicitada = string_new();
		if(args[1] != NULL)
		{
			tablaSolicitada = malloc(strlen(args[1]) + 1 );
			strcpy(tablaSolicitada, args[1]);
		}
		bool solicitadoPorMemoria = false;
		int problem = 0;
		if (0 == (problem = describirTablas(tablaSolicitada, solicitadoPorMemoria, NULL)))
		{
			logInfo( "Consola: Todas las tablas solicitadas fueron descritas correctamente");
		}
		else
		{

		}
		free(tablaSolicitada);
	}
}

void drop (char** args)
{
	logInfo( "Consola: Se ha solicitado realizar un DROP");
	if(chequearParametros(args, 2) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada\n");
		logError( "Consola: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* tablaAEliminar = malloc(strlen(args[1]) + 6);
		strcpy(tablaAEliminar, args[1]);
		int results = llamarEliminarTabla(tablaAEliminar);
		switch(results)
		{
		case 1:
		{
			printf("La tabla que usted deseaba eliminar no existe\n");
			logError( "Consola: Tabla inexistente");
			break;
		}
		case 0:
		{
			printf("La tabla ha sido eliminada exitosamente\n");
			logInfo( "Consola: Tabla eliminada correctamente");
			break;
		}
		default:
		{
			printf("Ocurrio un error al intentar eliminar la tabla deseada\n");
			logError( "Consola: operacion no terminada");
		}
		}
		free(tablaAEliminar);
	}
}

int chequearParametros(char** args, int cantParametros)
{
	int i;
	int parametroinvalido = 0;
	for(i = 1 ; i < cantParametros; i++)
	{
		if(args[i] == NULL || args[cantParametros] != NULL)
		{
			parametroinvalido = 1;
			break;
		}
		else
			parametroinvalido = 0;

	}
	return parametroinvalido;
}

