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
	puts("-_____________________________________________________");
	puts("CONSOLA");
	puts("------ Escriba un comando ------");
	puts("1. - SELECT <Table> <Key>");
	puts("2. - INSERT <Table> <Key> <Value> <Timestamp> (last one optional)");
	puts("3. - CREATE <Table> <Consistency> <Amount of partitions> <Time until next compaction>");
	puts("4. - DESCRIBE <Table> (Table is optional, if you want all tables to be shown, leave this parameter empty");
	puts("5. - DROP <Table>");
	char* linea;
//ejecutar prueba.txt
	while (1) {
		linea = readline("\nIngrese el comando a ejecutar con los parametros necesarios:\n ");

		if (strcmp(linea, "exit")==0){
			free(linea);
			puts("EXIT.");
			signalExit = true;
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

	char** args;
	if(!strcmp(comando->nombre, "INSERT"))
	{
		args = malloc(strlen(linea_aux) + 1);
		int a = 0;
		int b = 0;
		int c = 0;
		while(linea_aux[a] != '\0')
		{
			switch(linea_aux[a])
			{
				case ' ':
				{
					b++;
					a++;
					c = 0;
					break;
				}
				case '"':
				{
					a++;
					while(linea_aux[a] != '"')
					{

						args[b][c] = linea_aux[a];
						a++;
						c++;
					}
					a++;
					break;
				}
				default:
				{
					args[b][c] = linea_aux[a];
					a++;
					c++;
					break;
				}
			}
		}
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
		char* tabla = string_new();
		tabla = malloc(strlen(args[1]) + 1);
		strcpy(tabla, args[1]);
		char* claveaux = string_new();
		claveaux = malloc(strlen(args[2]) + 1);
		strcpy(claveaux, args[2]);
		uint16_t key = atoi(claveaux);
		t_keysetter* keysetterObtenido = selectKey(tabla, key);
		printf("La clave obtenida mas actualizada es %i,%lf,%s", keysetterObtenido->key, keysetterObtenido->timestamp, keysetterObtenido->clave);
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
		char* tabla = string_new();
		tabla = malloc(strlen(args[1]) + 1);
		strcpy(tabla, args[1]);
		char* claveaux = string_new();
		claveaux = malloc(strlen(args[2]) + 1);
		strcpy(claveaux, args[2]);
		uint16_t key = atoi(claveaux);
		char* value = string_new();
		value = malloc(strlen(args[3]) + 1);
		strcpy(value, args[3]);
		if(args[4] == NULL)
		{
			double timestampact = getCurrentTime();
			printf("Current time: %lf\n", timestampact);
			insertKeysetter(tabla, key, value, timestampact);
			logInfo( "Consola: Insert realizado.");
		}
		else
		{
			char* timestampaux = string_new();
			timestampaux = malloc(strlen(args[4]) + 1);
			strcpy(timestampaux, args[4]);
			double timestamp = atoi(timestampaux);
			insertKeysetter(tabla, key, value, timestamp);
			logInfo( "Consola: Insert realizado.");
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
		char* tabla = string_new();
		tabla = malloc(strlen(args[1]) + 1);
		strcpy(tabla, args[1]);
		char* consistencia = string_new();
		consistencia = malloc(strlen(args[2]) + 1);
		strcpy(consistencia, args[2]);
		char* particionesaux = string_new();
		particionesaux = malloc(strlen(args[3]) + 1);
		strcpy(particionesaux, args[3]);
		int particiones = atoi(particionesaux);
		char* intervaloCompactacionaux = string_new();
		intervaloCompactacionaux = malloc(strlen(args[4]) + 1);
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
		free(tabla);
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
		char* tablaAEliminar = string_new();
		tablaAEliminar = malloc(strlen(args[1]) + 6);
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
