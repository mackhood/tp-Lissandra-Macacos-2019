#include "Compactador.h"

void mainCompactador()
{
	//acceso a las tablas
	DIR* directorioDeTablas;
	struct dirent* tdp;
	char* auxdir = malloc(strlen(punto_montaje) + 8);//le sumo 8 por tables/ y 1 por las dudas(siempre asignar uno de mas)
	strcpy(auxdir, punto_montaje);
	strcat(auxdir, "Tables/");
	if(NULL == (directorioDeTablas = opendir(auxdir)))
	{
		logError( "FileSystem: error al acceder al directorio de tablas, abortando");
		free(auxdir);
		closedir(directorioDeTablas);
	}
	else

	{
		gestionarDumps();
		while(NULL != (tdp = readdir(directorioDeTablas)))//primero: while para asignar direcciones para funcion que crea hilos.
		{
			if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, ".."))	{}
			else
			{
				gestionarTabla(tdp->d_name);
			}

			//segundo: llamado a gestion de dumps.

		}
	}

}

void setearValoresCompactador(t_config* archivoConfig)
{
	tiempoDump = config_get_int_value(archivoConfig, "TIEMPO_DUMP");
	tablasEnEjecucion = list_create();
	slowestCompactationInterval = 0;
	deathProtocol = 0;
	lastDumpSituation = 0;
}

void gestionarTabla(char*tabla)
{
	pthread_t hiloTabla;
	logInfo("Compactador: Se inicio un hilo para manejar a %s.",tabla);
	pthread_create(&hiloTabla, NULL, (void *) compactarTablas, tabla);
	pthread_detach(hiloTabla);
}

void compactarTablas(char*tabla)
{
	bool estaTabla(t_TablaEnEjecucion* tablaDeLista)
	{
		char* tablaAux = malloc(strlen(tabla) + 1);
		strcpy(tablaAux, tabla);
		int cantCarac = strlen(tablaDeLista->tabla);
		//char* tablaDeListaAux = string_new();
		char* tablaDeListaAux = malloc(cantCarac + 1);
		strcpy(tablaDeListaAux, tablaDeLista->tabla);
		bool result = (0 == strcmp(tablaDeListaAux, tablaAux));
		free(tablaDeListaAux);
		free(tablaAux);
		return result;
	}
	t_TablaEnEjecucion* tablaAAgregar = malloc(sizeof(t_TablaEnEjecucion) + 4);
	tablaAAgregar->tabla = tabla;
	char* direccionTabla = malloc(strlen(punto_montaje) + strlen(tabla) + 10);//son 20 de tables/ y metadata.cfg +1 por las dudas
	strcpy(direccionTabla, punto_montaje);
	strcat(direccionTabla, "Tables/");
	strcat(direccionTabla, tabla);
	strcat(direccionTabla, "/");
	DIR* tablaAccedida = opendir(direccionTabla);
	struct dirent* tbp;
	int cantTempsActuales = 0;
	while(NULL != (tbp = readdir(tablaAccedida)))
	{
		if(string_ends_with(tbp->d_name, ".tmp"))
		{
			cantTempsActuales++;
		}
	}
	free(tbp);
	closedir(tablaAccedida);
	free(direccionTabla);
	tablaAAgregar->cantTemps = cantTempsActuales;
	list_add(tablasEnEjecucion, tablaAAgregar);
	char* direccionMetadataTabla = malloc(strlen(punto_montaje) + strlen(tabla) + 21);//son 20 de tables/ y metadata.cfg +1 por las dudas
	strcpy(direccionMetadataTabla, punto_montaje);
	strcat(direccionMetadataTabla, "Tables/");
	strcat(direccionMetadataTabla, tabla);
	strcat(direccionMetadataTabla, "/Metadata.cfg");
	t_config * temporalArchivoConfig;
	temporalArchivoConfig = config_create(direccionMetadataTabla);
	int tiempoEntreCompactacion = config_get_int_value(temporalArchivoConfig, "TIEMPOENTRECOMPACTACIONES");
	if(tiempoEntreCompactacion > slowestCompactationInterval)
		slowestCompactationInterval = tiempoEntreCompactacion;
	config_destroy(temporalArchivoConfig);
	while(1)
	{
		usleep(tiempoEntreCompactacion * 1000);
		if(!list_find(tablasEnEjecucion, (void*) estaTabla))
		{
			logInfo("Compactador: La %s, al haber sido previamente desalojada dejará de ser buscada en el compactador", tabla);
			break;
		}
		else
		{
			//ejecutarCompactacion(tabla);
		}
	}
	free(direccionMetadataTabla);
}

void gestionarDumps()
{
	pthread_t hiloMemtable;
	logInfo("Compactador: Se inicio un hilo para manejar a la memtable");
	pthread_create(&hiloMemtable, NULL, (void *) gestionarMemtable, NULL);
	pthread_detach(hiloMemtable);
}

void gestionarMemtable()
{
	while(!lastDumpSituation)
	{
		usleep(tiempoDump * 1000);
		int a = 0;
		while(NULL != list_get(tablasEnEjecucion, a))
		{
			t_TablaEnEjecucion* tablaTomada = list_get(tablasEnEjecucion, a);
			crearTemporal(tablaTomada->tabla);
			a++;
		}
		if(deathProtocol){}
		else
		{
//			list_destroy(memtable);
//			memtable = list_create();
			list_clean(memtable);
		}
	}
}

void crearTemporal(char* tabla)
{
	int perteneceATabla(t_Memtablekeys* key)
	{
		char* testTable = malloc(strlen(tabla) + 1);
		strcpy(testTable, tabla);
		return 0 == strcmp(key->tabla, testTable);
	}

	bool estaTabla(t_TablaEnEjecucion* tablaDeLista)
	{
		char* tablaAux = malloc(strlen(tabla) + 1);
		strcpy(tablaAux, tabla);
		int cantCarac = strlen(tablaDeLista->tabla);
		//char* tablaDeListaAux = string_new();
		char* tablaDeListaAux = malloc(cantCarac + 1);
		strcpy(tablaDeListaAux, tablaDeLista->tabla);
		bool result = (0 == strcmp(tablaDeListaAux, tablaAux));
		free(tablaDeListaAux);
		free(tablaAux);
		return result;
	}

	t_list* keysTableSpecific = list_create();
	keysTableSpecific = list_filter(memtable, (void*)perteneceATabla);
	size_t sizeOfContainer = list_size(keysTableSpecific)*(tamanio_value + 24 + 15 + 4);
	char* container = malloc(sizeOfContainer + 1);
	t_TablaEnEjecucion* tablaEjecutada = list_find(tablasEnEjecucion, (void*) estaTabla);
	t_Memtablekeys* auxiliaryKey;
	int a = 0;
	FILE* tempPointer;
	char* auxTempDir = string_itoa(tablaEjecutada->cantTemps);
	char* tempDirection = malloc(strlen(tabla) + strlen(punto_montaje) + 8 + strlen(auxTempDir) + 5);
	strcpy(tempDirection, punto_montaje);
	strcat(tempDirection, "Tables/");
	strcat(tempDirection, tabla);
	strcat(tempDirection, "/");
	strcat(tempDirection, string_itoa(tablaEjecutada->cantTemps));
	strcat(tempDirection, ".tmp");
	bool firstRun = true;
	int usedSize = 0;
	while(NULL != list_get(keysTableSpecific, a))
	{
		logInfo("Compactador: se iniciar el proceso de crear un .tmp nuevo a la %s", tabla);
		if(firstRun)
		{
			tempPointer = fopen(tempDirection, "w+");
			char* size = string_new();
			size = malloc(7);
			strcpy(size, "SIZE=");
			strcat(size, "\n");
			fwrite(size, strlen(size), 1, tempPointer);
			char* blocks = string_new();
			blocks = malloc (9);
			strcpy(blocks, "BLOCKS=");
			strcat(blocks, "\n");
			fwrite(blocks, strlen(blocks), 1, tempPointer);
			fclose(tempPointer);
			tablaEjecutada->cantTemps++;
		}
		auxiliaryKey = malloc(sizeof(t_Memtablekeys) + 4);
		auxiliaryKey = list_get(keysTableSpecific, a);
		int sizeOfKey = strlen(string_itoa(auxiliaryKey->data->key)) + strlen(auxiliaryKey->data->clave)
				+ strlen(string_from_format("%lf", auxiliaryKey->data->timestamp)) + 3;
		char* claveParaTemp = malloc(sizeOfKey + 1);
		char* auxTimestamp = string_from_format("%lf", auxiliaryKey->data->timestamp);
		strcpy(claveParaTemp, string_substring_until(auxTimestamp, strlen(auxTimestamp) - 7));
		strcat(claveParaTemp, ";");
		strcat(claveParaTemp, string_itoa(auxiliaryKey->data->key));
		strcat(claveParaTemp, ";");
		strcat(claveParaTemp, auxiliaryKey->data->clave);
		strcat(claveParaTemp, "\n");
		if(firstRun)
		{
			strcpy(container, claveParaTemp);
			firstRun = false;
		}
		else
			strcat(container, claveParaTemp);
		usedSize += strlen(claveParaTemp);
		free(claveParaTemp);
		a++;
	}
	if(!firstRun)
	{
		t_config* tempArchConf;
		tempArchConf = config_create(tempDirection);
		char* sizedUse = string_itoa(usedSize);
		config_set_value(tempArchConf, "SIZE", sizedUse);
		char* bloquesAsignados = escribirBloquesDeFs(container, usedSize, tabla);
		config_set_value(tempArchConf, "BLOCKS", bloquesAsignados);
		config_save(tempArchConf);
		logInfo("Compactador: temporal creado");
		config_destroy(tempArchConf);
	}
	free(tempDirection);
	list_destroy(keysTableSpecific);
}

void ejecutarCompactacion(char* tabla)
{
	while(selectActivo){}
	pthread_mutex_lock(&compactacionActiva);
	char* direccionTabla = malloc(strlen(tabla) + strlen(punto_montaje) + 9);
	strcpy(direccionTabla, punto_montaje);
	strcat(direccionTabla, "Tables/");
	strcat(direccionTabla, tabla);
	strcat(direccionTabla, "/");
	struct dirent* tdp;
	DIR* tableDirectory = opendir(direccionTabla);
	while(NULL != (tdp = readdir(tableDirectory)))
	{
		t_list* keysToManage = list_create();
		if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, "..")){}
		else
		{
			bool firstRead = true;
			if(string_ends_with(tdp->d_name, ".tmp"))
			{
				char* direccionTemp = malloc(strlen(direccionTabla) + strlen(tdp->d_name) + 1);
				strcpy(direccionTemp, direccionTabla);
				strcat(direccionTemp, tdp->d_name);
				int fullTempSize = obtenerTamanioArchivoConfig(direccionTemp);
				char** blocks = obtenerBloques(direccionTemp);
				int counter = 0;
				int alreadyCountedSize = 0;
				char* keysToParse = malloc(fullTempSize + 1);
				while(blocks[counter] != NULL)
				{
					char* blockDirection = malloc(strlen(direccionFileSystemBlocks) + strlen(blocks[counter]) + 5);
					strcpy(blockDirection, direccionFileSystemBlocks);
					strcat(blockDirection, blocks[counter]);
					strcat(blockDirection, ".bin");
					FILE* blockPointer = fopen(blockDirection, "r+");
					int sizeToRead;
					if((fullTempSize - alreadyCountedSize) > tamanio_bloques)
						sizeToRead = tamanio_bloques;
					else
						sizeToRead = fullTempSize - alreadyCountedSize;
					char* blockContents = malloc(sizeToRead + 1);
					fread(blockContents, sizeToRead, 1, blockPointer);
					if(firstRead)
					{
						strcpy(keysToParse, blockContents);
						firstRead = false;
					}
					else
						strcat(keysToParse, blockContents);
					fclose(blockPointer);
					free(blockContents);
					free(blockDirection);
				}

			}
		}
	}
	pthread_mutex_unlock(&compactacionActiva);
}

void killProtocolCompactador()
{
	deathProtocol = 1;
	lastDumpSituation = 1;
	list_clean(tablasEnEjecucion);
	logInfo("Compactador: Desconectando todas las tablas.");
	usleep(slowestCompactationInterval * 1000); //Esto está para que el compactador tenga tiempo a matar todas las tablas de su sistema.
}

