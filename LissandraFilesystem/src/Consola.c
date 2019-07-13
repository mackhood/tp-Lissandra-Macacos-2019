#include "Consola.h"

COMANDO comandos[] = {
		{"SELECT",selectt},
		{"INSERT",insert},
		{"CREATE",create},
		{"DESCRIBE",describe},
		{"DROP",drop},
		{"DETAILS", details},
		{"SHOWMENU", show_menu},
		{"MODIFYDUMP", modifyDumpTime},
		{"MODIFYDELAY", modifyRetardo},
		{(char *) NULL, (Funcion *) NULL}
};

void consola()
{
	printf("\033[1;33m");
	puts("¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤");
	puts("-------------------------------------Lissandra FileSystem-------------------------------------------");
	puts("------------------------------------- Escriba un comando -------------------------------------------");
	printf("\033[0;33m");
	puts("1. - SELECT <Table> <Key>");
	puts("2. - INSERT <Table>|<Key>|<Value>|<Timestamp[milliseconds]> (last one is optional)");
	puts("3. - CREATE <Table> <Consistency: EC/SC/SHC> <Amount of partitions> <Time until next compaction>");
	puts("4. - DESCRIBE <Table> (Table is optional, if you want all tables to be shown, leave this parameter empty)");
	puts("5. - DROP <Table>");
	puts("6. - DETAILS (Explains how to use the File System's Interface.).");
	puts("7. - SHOWMENU");
	puts("8. - MODIFYDUMP <NewDumpTime>");
	puts("9. - MODIFYDELAY <NewDelayTime>");
	puts("10. - EXIT");
	printf("\033[1;31m");
	puts("\nAdvertencia: los pipes solo deben ser usados para separar los parámetros del Insert, en otras instrucciones"
			" no son considerados.");
	char* linea;
//ejecutar prueba.txt
	while (1) {
		printf("\033[1;36m");
		linea = readline("\nIngrese el comando a ejecutar con los parámetros necesarios:\n ");
		char* aux = malloc(strlen(linea) + 1);
		strcpy(aux, linea);
		string_to_lower(aux);
		if (!strcmp(aux, "exit"))
		{
			free(linea);
			printf("\033[1;34m");
			puts("El FileSystem procederá a apagarse.");
			printf("\033[0m");
			clear_history();
			free(aux);
			pthread_mutex_unlock(&deathProtocol);
			break;
		}
		printf("\033[1;34m");
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

		usleep(retardo * 1000);
	}
	else
	{
		puts("El comando que quiere ejecutar no tiene el formato especificado.");
		free(funcion);
		free(linea_aux);
	}
	return 1;
}

void selectt (char** args)
{
	logInfo( "[Consola]: Se ha recibido un pedido de select.");
	if(chequearParametros(args, 3) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError("[Consola]: solicitud posee cantidad errónea de parámetros");
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
			t_keysetter* keysetterObtenido = selectKey(tabla, key);
			if(keysetterObtenido != NULL)
				printf("La clave obtenida mas actualizada es %i,%lf,%s\n", keysetterObtenido->key, keysetterObtenido->timestamp, keysetterObtenido->clave);
			free(tabla);
			free(claveaux);
		}
	}
}

void insert (char** args)
{
	logInfo( "[Consola]: Se ha recibido un pedido de insert.");
	int chequeo = 0;
	if(args[4] == NULL )
		chequeo = chequearParametros(args, 4);
	else
		chequeo = chequearParametros(args, 5);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(!itsANumber(args[2]))
		{
			puts("La key que ingresó posee caracteres inválidos.");
			logError( "[Consola]: la key a Insertar es inválida.");
		}
		else if(strlen(args[2]) > 5)
		{
			puts("La key que ingresó es demasiado grande.");
			logError( "[Consola]: la key a Insertar es demasiado grande.");
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
				logError( "[Consola]: la key a Insertar es inválida.");
				free(tabla);
				free(claveaux);
				return;
			}
			uint16_t key = atoi(claveaux);
			if(string_contains(args[3], ";"))
			{
				printf("Por favor, ingrese un value que no tenga ';' dentro.");
				logError("[Consola]: se ingresó un value inválido");
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
					if(!itsANumber(args[4]))
					{
						puts("El timestamp ingresado contiene caracteres inválidos.");
						logError( "[Consola]: el timestamp a Insertar es inválido.");
						free(value);
						free(claveaux);
						free(tabla);
						return;
					}
					else
					{
						char* timestampaux = malloc(strlen(args[4]) + 1);
						strcpy(timestampaux, args[4]);
						if(strlen(timestampaux) > 15)
						{
							puts("Ha insertado un timestamp demasiado grande. Por favor, ingrese uno más pequeño.");
							logError( "[Consola]: el timestamp a Insertar es inválido.");
							free(value);
							free(claveaux);
							free(tabla);
							free(timestampaux);
						}
						else
						{
							double timestamp = atoi(timestampaux);
							result = insertKeysetter(tabla, key, value, timestamp);
							free(timestampaux);
						}
					}
				}
				switch(result)
				{
				case 0:
				{
					logInfo( "[Consola]: Insert realizado.");
					free(claveaux);
					break;
				}
				case 1:
				{
					logError("[Consola]: Insert fallido por tabla inexistente");
					free(value);
					free(claveaux);
					free(tabla);
					break;
				}
				case 2:
				{
					logError("[Consola]: la memtable no pudo recibir la clave");
					free(value);
					free(claveaux);
					free(tabla);
					break;
				}
				case 3:
				{
					logError("[Consola]: el value era demasiado grande para realizar el insert");
					free(value);
					free(claveaux);
					free(tabla);
					break;
				}
				}
			}
		}
	}
}

void create (char** args)
{
	logInfo( "[Consola]: Se ha recibido un pedido de create.");
	if(chequearParametros(args, 5) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(criterioInvalido(args[2]))
		{
			puts("El criterio es inválido.");
			logError( "[Consola]: el criterio de CREATE es inválido.");
		}
		else if(!itsANumber(args[3]))
		{
			puts("La cantidad de particiones es inválida.");
			logError( "[Consola]: la cantidad de particiones para CREATE es inválida.");
		}
		else if(atoi(args[3]) < 1)
		{
			puts("La cantidad de particiones es inválida.");
			logError( "[Consola]: la cantidad de particiones para CREATE es inválida.");
		}
		else if(!itsANumber(args[4]))
		{
			puts("El tiempo entre cada compactación es inválido.");
			logError( "[Consola]: el tiempo entre cada compactación para CREATE es inválido.");
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
					logInfo( "[Consola]: Tabla creada satisfactoriamente");
					break;
				}
				case 2:
				{
					printf("La tabla solicitada ya existe\n");
					logInfo( "[Consola]: Tabla ya existía");
					break;
				}
				case 5:
				{
					puts("No hay suficientes bloques disponibles en el FS para crear esta tabla");
					logError("[Consola]: Se solicitaron más bloques de los disponibles actualmente en el FS.");
					break;
				}
				default:
				{
					printf("Error al crear la tabla\n");
					logError( "[Consola]: La tabla o alguna de sus partes no pudo ser creada");
					break;
				}
			}
			free(consistencia);
			free(particionesaux);
			free(intervaloCompactacionaux);
		}
	}
}

void describe (char** args)
{
	logInfo( "[Consola]: Se ha recibido un pedido de describe.");
	int chequeo = 0;
	if(args[1] == NULL )
		chequeo = chequearParametros(args, 1);
	else
		chequeo = chequearParametros(args, 2);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
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
		char* problem = malloc(2);
		if (!strcmp((problem = describirTablas(tablaSolicitada, solicitadoPorMemoria)), "0"))
		{
			logInfo( "[Consola]: Todas las tablas solicitadas fueron descritas correctamente");
		}
		else
		{
			logError("[Consola]: No pudieron ser mostradas las tablas solicitadas");
		}
		free(tablaSolicitada);
	}
}

void drop (char** args)
{
	logInfo( "[Consola]: Se ha solicitado realizar un DROP");
	if(chequearParametros(args, 2) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
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
			logError( "[Consola]: Tabla inexistente");
			break;
		}
		case 0:
		{
			printf("La tabla ha sido eliminada exitosamente\n");
			logInfo( "[Consola]: Tabla eliminada correctamente");
			break;
		}
		default:
		{
			printf("Ocurrió un error al intentar eliminar la tabla deseada\n");
			logError( "[Consola]: operacion no terminada");
		}
		}
		free(tablaAEliminar);
	}
}

void details(char** args)
{
	logInfo("[Consola]: se ha solicitado información sobre le uso del FileSystem.");
	int chequeo = chequearParametros(args, 1);
	if(chequeo)
	{
		printf("La instrucción details no lleva parámetros.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		puts("¤ Bienvenido a Lissandra File System. ¤");
		puts("  ¤¤ Instrucciones para el uso correcto y eficiente del FS, especificado por cada instrucción.");
		puts("  ¤¤ Instrucciones generales:");
		puts("Los parámetros de las instrucciones son bastante sencillos. Tanto los nombres de las tablas como "
				"los comandos pueden ser escritos con mayusculas y minúsculas como se desee, ya que el mismo es case sensitive.");
		puts("Se sugiere no modificar los contenidos de las carpetas donde el FS trabaja durante la ejecución del mismo,"
				" semejante acción podría llevar a que sucedan errores inesperados.");
		puts("  ¤¤ Manejo de Comandos:");
		puts("   1¤¤¤ Select:");
		puts("El comando Select buscará el valor de la clave más actualizado que se encuentre en este momento dentro del FS."
				"Posee dos parámetros:");
		puts("      Nombre de tabla, el cual puede tener cualquier caracter menos espacios.");
		puts("      Key, un int que es el patrón de referencia que usará el resto de las operaciones para ubicar la key en el FS.");
		puts("Para funcionar, la tabla deberá existir en el sistema y la clave debe haber sido impactada en ella, o"
				" existir en la memtable.");
		puts("El nombre de la tabla puede contener cualquier caracter menos espacios, sin embargo, la key debe ser únicamente "
				"numérica, caso contrario será notificado para que la corrija.");
		puts("   2¤¤¤ Insert:");
		puts("El comando Insert crea una nueva key a insertar dentro del File System. Para ello, se debe pasar los parámetros"
				" solicitados en el orden como se indica en el menú de la consola, y siendo separados por un pipe '|'.");
		puts("Los parámetros de éste comando son:");
		puts("      Nombre de tabla, el cual puede tener cualquier caracter menos espacios.");
		puts("      Key, un int que es el patrón de referencia que usará el resto de las operaciones para ubicar la key en el FS.");
		puts("      Value, un string que puede poseer una cantidad limitada de caracteres y representa el valor de la Key en el FS.");
		puts("      Timestamp, un double que corresponde a la hora en que fue creada ésta key. Timestamp es un parámetro opcional"
				", con lo cual en caso de no serle otorgado uno manualmente, el FS le asignará el Timestamp actual.");
		puts("Insert tomará la clave y la llevará a la memtable, donde la misma estará una cantidad de tiempo determinada por"
				" el archivo de configuración. Una vez transcurrido ese lapso, la clave será impactada en un archivo temporal, "
				"para luego ser compactada y comparada en caso de que la key ya exista en el FS.");
		puts("   3¤¤¤ Create:");
		puts("El comando Create es el encargado de crear las tablas en el FS. Encargándose de asignarle un bloque libre "
				"del FS a cada partición de la tabla en el momento de su creación, además de ponerla en la lista de tablas"
				" en ejecución y prepararla para las posibles compactaciones a ocurrir. El comando Create tiene 4 parámetros:");
		puts("      Nombre de la tabla, puede contener cualquier caractér (salvo espacios), es el nombre que tendrá el "
				"directorio donde resida la información de la tabla de ahora en adelante.");
		puts("      Consistencia: La consistencia tiene 3 tipos. Cada tipo de consistencia especificará el nivel de renovación"
				" de los datos al momento de hacerle una solicitud al FS. SC es aquella en la cual las solicitudes siempre obtienen"
				" los resultados más actualizados, sin embargo, tiene un único origen. EC obtiene una mayor cantidad de orígenes"
				" para revisar la información, pero suele ser menos preciso en cuanto a que tan nuevos son esos datos. SHC funciona"
				"de manera similar a SC, solo que toma la ventaja de usar múltiples memorias. Para ésto se usara una función"
				" 'HASH' para determinar a que memoria irá la key."
				" ");
		puts("      Cantidad de particiones, un int que determina cuántas particiones tendrá la tabla en el FS.");
		puts("      Tiempo entre compactaciones, este valor, un int, determina cuánto tiempo pasará entre compactación y compac"
				"tación. Cada compactación ayudará a eliminar los archivos temporales y a actualizar las keys ya existentes en el"
				" FS");
		puts("Como único requisito, la tabla que uno desee crear no debe existir previamente en el FS, en caso contrario, será"
				" notificado para sobre la situación.");
		puts("   4¤¤¤ Describe:");
		puts("El comando Describe tiene como funcionalidad mostrar la metadata de una tabla específica, o de todas las tablas"
				" que actualmente se encuentren en el FS. Posee un único parámetro, que es opcional:");
		puts("      Nombre de la tabla, el nombre de la tabla puede tener cualquier caracter menos espacios. En caso de dejar"
				" este campo en blanco. El FS devolverá la información de la metadata de todas las tablas existentes");
		puts("Como único requisito, se tiene el de que en caso de pedir el de una tabla específica, que esa tabla exista en el FS");
		puts("   5¤¤¤ Drop:");
		puts("El comando Drop se encarga de eliminar las tablas de adentro del FS, destruyendo toda la información que la misma"
				" haya almacenado en los bloques del FS. Drop posee un solo parámetro:");
		puts("      Nombre de la tabla a eliminar, el nombre de la tabla puede tener cualquier caracter menos espacios.");
		puts("El único requisito de Drop es que la tabla a eliminar exista en el FS, caso contrario, se notificará sobre su inexi"
				"stencia.");
		puts("   6¤¤¤ Exit:");
		puts("El comando Exit inicia el procedimiento de fin del FS, destruyendo estructuras temporales, desalojando las memorias"
				" y realizando los pasos necesarios para afianzar los datos que hayan quedado sin impactar.");
		puts("Se agradece que tome en cuenta las información dada sobre el funcionamiento del FS");
		printf("\033[1;33m");
		puts("¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤");
		puts("-------------------------------------Lissandra FileSystem-------------------------------------------");
		puts("------------------------------------- Escriba un comando -------------------------------------------");
		printf("\033[0;33m");
		puts("1. - SELECT <Table> <Key>");
		puts("2. - INSERT <Table>|<Key>|<Value>|<Timestamp[milliseconds]> (last one is optional)");
		puts("3. - CREATE <Table> <Consistency: EC/SC/SHC> <Amount of partitions> <Time until next compaction>");
		puts("4. - DESCRIBE <Table> (Table is optional, if you want all tables to be shown, leave this parameter empty)");
		puts("5. - DROP <Table>");
		puts("6. - DETAILS (Explains how to use the File System's Interface.).");
		puts("7. - SHOWMENU");
		puts("8. - MODIFYDUMP <NewDumpTime>");
		puts("9. - MODIFYDELAY <NewDelayTime>");
		puts("10. - EXIT");

		printf("\033[1;31m");
		puts("\nAdvertencia: los pipes solo deben ser usados para separar los parámetros del Insert, en otras instrucciones"
				" no son considerados.");
	}
}

void show_menu(char** args)
{
	int chequeo = chequearParametros(args, 1);
	if(chequeo)
	{
		printf("La instrucción SHOW_MENU no lleva parámetros.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		printf("\033[1;33m");
		puts("¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤º°`°º¤ø,¸¸,ø¤º°`°º¤ø,¸,ø¤°º¤ø,¸¸,ø¤");
		puts("-------------------------------------Lissandra FileSystem-------------------------------------------");
		puts("------------------------------------- Escriba un comando -------------------------------------------");
		printf("\033[0;33m");
		puts("1. - SELECT <Table> <Key>");
		puts("2. - INSERT <Table>|<Key>|<Value>|<Timestamp[milliseconds]> (last one is optional)");
		puts("3. - CREATE <Table> <Consistency: EC/SC/SHC> <Amount of partitions> <Time until next compaction>");
		puts("4. - DESCRIBE <Table> (Table is optional, if you want all tables to be shown, leave this parameter empty)");
		puts("5. - DROP <Table>");
		puts("6. - DETAILS (Explains how to use the File System's Interface.).");
		puts("7. - SHOWMENU");
		puts("8. - MODIFYDUMP <NewDumpTime>");
		puts("9. - MODIFYDELAY <NewDelayTime>");
		puts("10. - EXIT");
		printf("\033[1;31m");
		puts("\nAdvertencia: los pipes solo deben ser usados para separar los parámetros del Insert, en otras instrucciones"
				" no son considerados.");
	}
}

void modifyDumpTime(char** args)
{
	logInfo("[Consola]: Ha llegado un pedido para modificar el tiempoDeDump.");
	int chequeo = chequearParametros(args, 2);
	if(chequeo)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError("[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else if(!itsANumber(args[1]))
	{
		puts("El valor que ingresó tiene caracteres inválidos.");
		logError( "[Consola]: el valor para modificar es inválido.");
	}
	else
	{
		t_config* ConfigMain = config_create(lissandraFL_config_ruta);
		char* dumpTime = malloc(strlen(args[1]) + 1);
		strcpy(dumpTime, args[1]);
		config_set_value(ConfigMain, "TIEMPO_DUMP", dumpTime);
		config_save(ConfigMain);
		config_destroy(ConfigMain);
		free(dumpTime);
		logInfo("[Consola]: se ha modificado el tiempo de dumpeo.");
		puts("Se ha modificado el tiempo entre Dumps");
	}
}

void modifyRetardo(char** args)
{
	logInfo("[Consola]: Ha llegado un pedido para modificar el retardo entre instrucciones.");
	int chequeo = chequearParametros(args, 2);
	if(chequeo)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError("[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else if(!itsANumber(args[1]))
	{
		puts("El valor que ingresó tiene caracteres inválidos.");
		logError( "[Consola]: el valor para modificar es inválido.");
	}
	else
	{
		t_config* ConfigMain = config_create(lissandraFL_config_ruta);
		char* delay = malloc(strlen(args[1]) + 1);
		strcpy(delay, args[1]);
		config_set_value(ConfigMain, "RETARDO", delay);
		config_save(ConfigMain);
		config_destroy(ConfigMain);
		free(delay);
		logInfo("[Consola]: se ha modificado el tiempo de delay.");
		puts("Se ha modificado el delay entre instrucciones");
	}
}

