#include "Compactador.h"

void mainCompactador()
{
	//acceso a las tablas
	direccionDeLasTablas = malloc(strlen(punto_montaje) + 8);//le sumo 8 por tables/ y 1 por las dudas(siempre asignar uno de mas)
	strcpy(direccionDeLasTablas, punto_montaje);
	strcat(direccionDeLasTablas, "Tables/");
	if(NULL == (directorioDeTablas = opendir(direccionDeLasTablas)))
	{
		logError("[Compactador]: error al acceder al directorio de tablas, abortando");
		free(direccionDeLasTablas);
		closedir(directorioDeTablas);
	}
	else
	{
		gestionarDumps();
		while(NULL != (TableEntry = readdir(directorioDeTablas)))//primero: while para asignar direcciones para funcion que crea hilos.
		{
			if(!strcmp(TableEntry->d_name, ".") || !strcmp(TableEntry->d_name, ".."))	{}
			else
			{
				gestionarTabla(TableEntry->d_name);
			}
		}
	}
}

void setearValoresCompactador(t_config* archivoConfig)
{
	tiempoDump = config_get_int_value(archivoConfig, "TIEMPO_DUMP");
	tablasEnEjecucion = list_create();
	slowestCompactationInterval = tiempoDump;
	lastDumpSituation = 0;
}

void gestionarTabla(char*tabla)
{
	pthread_t hiloTabla;
	logInfo("[Compactador]: Se inicio un hilo para manejar a %s.",tabla);
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
	t_TablaEnEjecucion* tablaAAgregar = malloc(sizeof(t_TablaEnEjecucion) + 2);
	tablaAAgregar->tabla = tabla;
	pthread_mutex_init(&tablaAAgregar->compactacionActiva, NULL);
	pthread_mutex_init(&tablaAAgregar->renombreEnCurso, NULL);
	pthread_mutex_init(&tablaAAgregar->dropPendiente, NULL);
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
			logInfo("[Compactador]: La %s, al haber sido previamente desalojada dejará de ser buscada en el compactador", tabla);
			break;
		}
		else if(tablaAAgregar->cantTemps == 0){}
		else
		{
			pthread_mutex_lock(&tablaAAgregar->dropPendiente);
			ejecutarCompactacion(tabla);
			pthread_mutex_unlock(&tablaAAgregar->dropPendiente);
		}
	}
	free(direccionMetadataTabla);
}

void gestionarDumps()
{
	pthread_t hiloMemtable;
	logInfo("[Compactador]: Se inicio un hilo para manejar a la memtable");
	pthread_create(&hiloMemtable, NULL, (void *) gestionarMemtable, NULL);
	pthread_detach(hiloMemtable);
}

void gestionarMemtable()
{
	while(!lastDumpSituation)
	{
		usleep(tiempoDump * 1000);
		int a = 0;
		if(!list_is_empty(memtable))
		{
			int cantBlocksLibre = cantidadDeBloquesLibres();
			int cantBlocksNecesarios = cantidadDeBloquesAOcupar();
			if(cantBlocksNecesarios > cantBlocksLibre)
				fileSystemFull = true;
			else
				fileSystemFull = false;
			if(!fileSystemFull)
			{
				pthread_mutex_lock(&dumpEnCurso);
				logInfo("[Compactador]: Se procede a realizar un dump de la memtable.");
				while(NULL != list_get(tablasEnEjecucion, a))
				{
					t_TablaEnEjecucion* tablaTomada = list_get(tablasEnEjecucion, a);
					crearTemporal(tablaTomada->tabla);
					a++;
				}
				list_clean_and_destroy_elements(memtable, (void*)&liberadorDeMemtableKeys);
				pthread_mutex_unlock(&dumpEnCurso);
			}
			else
			{
				logInfo("[Compactador]: no hay suficiente espacio en el FS para realizar un Dump. Se reintentará luego.");
				puts("FileSystem se encuentra sin espacio suficiente para dumpear");
			}
		}
	}
}

void crearTemporal(char* tabla)
{
	int perteneceATabla(t_Memtablekeys* key)
	{
		char* testTable = malloc(strlen(tabla) + 1);
		strcpy(testTable, tabla);
		int result = strcmp(key->tabla, testTable);
		free(testTable);
		return 0 == result;
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
	t_TablaEnEjecucion* tablaEjecutada = list_find(tablasEnEjecucion, (void*) estaTabla);
	t_list* keysTableSpecific = list_filter(memtable, (void*)perteneceATabla);
	size_t sizeOfContainer = list_size(keysTableSpecific)*(tamanio_value + 24 + 17 + 5 + 3);
	char* container = malloc(sizeOfContainer + 1);
	t_Memtablekeys* auxiliaryKey;
	int a = 0;
	FILE* tempPointer;
	bool firstRun = true;
	int usedSize = 0;
	pthread_mutex_lock(&tablaEjecutada->renombreEnCurso);
	char* auxTempDir = string_itoa(tablaEjecutada->cantTemps);
	char* tempDirection = malloc(strlen(tabla) + strlen(punto_montaje) + 8 + strlen(auxTempDir) + 5);
	strcpy(tempDirection, punto_montaje);
	strcat(tempDirection, "Tables/");
	strcat(tempDirection, tabla);
	strcat(tempDirection, "/");
	strcat(tempDirection, auxTempDir);
	strcat(tempDirection, ".tmp");
	free(auxTempDir);
	logInfo("[Compactador]: se iniciar el proceso de crear un .tmp nuevo a la %s", tabla);
	while(NULL != list_get(keysTableSpecific, a))
	{
		if(firstRun)
		{
			tempPointer = fopen(tempDirection, "w+");
			char* size = malloc(7);
			strcpy(size, "SIZE=");
			strcat(size, "\n");
			fwrite(size, strlen(size), 1, tempPointer);
			char* blocks = malloc (9);
			strcpy(blocks, "BLOCKS=");
			strcat(blocks, "\n");
			fwrite(blocks, strlen(blocks), 1, tempPointer);
			fclose(tempPointer);
			free(blocks);
			free(size);
			tablaEjecutada->cantTemps++;
		}
		auxiliaryKey = list_get(keysTableSpecific, a);
		char* auxTimestamp = string_from_format("%lf", auxiliaryKey->data->timestamp);
		char* auxc = string_itoa(auxiliaryKey->data->key);
		int sizeOfKey = strlen(auxc) + strlen(auxiliaryKey->data->clave)
								+ strlen(auxTimestamp) + 3;
		char* claveParaTemp = malloc(sizeOfKey + 1);
		char* auxb = string_substring_until(auxTimestamp, strlen(auxTimestamp) - 7);
		strcpy(claveParaTemp, auxb);
		strcat(claveParaTemp, ";");
		strcat(claveParaTemp, auxc);
		strcat(claveParaTemp, ";");
		strcat(claveParaTemp, auxiliaryKey->data->clave);
		strcat(claveParaTemp, "\n");
		free(auxb);
		free(auxc);
		free(auxTimestamp);
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
		logInfo("[Compactador]: temporal creado");
		config_destroy(tempArchConf);
		free(sizedUse);
		free(bloquesAsignados);
	}
	pthread_mutex_unlock(&tablaEjecutada->renombreEnCurso);
	free(container);
	free(tempDirection);
	list_destroy(keysTableSpecific);
}

void ejecutarCompactacion(char* tabla)
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
	logInfo("[Compactador]: se procede a realizar la compactación de la %s", tabla);
	char* direccionTabla = malloc(strlen(tabla) + strlen(punto_montaje) + 9);
	strcpy(direccionTabla, punto_montaje);
	strcat(direccionTabla, "Tables/");
	strcat(direccionTabla, tabla);
	strcat(direccionTabla, "/");
	t_TablaEnEjecucion* tablaEspecifica = list_find(tablasEnEjecucion, (void*) estaTabla);
	struct dirent* tdp;
	DIR* tableDirectory = opendir(direccionTabla);
	t_list* keysToManage = list_create();
	t_list* keysPostParsing;
	int i = 0;
	pthread_mutex_lock(&tablaEspecifica->renombreEnCurso);
	for(i = 0; i < tablaEspecifica->cantTemps; i++)
	{
		char* aux = string_itoa(i);
		char* direccionARenombrar = malloc(strlen(direccionTabla) + strlen(aux) + 5);
		char* direccionFinal = malloc(strlen(direccionTabla) + strlen(aux) + 6);
		strcpy(direccionARenombrar, direccionTabla);
		strcat(direccionARenombrar, aux);
		strcat(direccionARenombrar, ".tmp");
		strcpy(direccionFinal, direccionTabla);
		strcat(direccionFinal, aux);
		strcat(direccionFinal, ".tmpc");
		rename(direccionARenombrar, direccionFinal);
		free(aux);
		free(direccionARenombrar);
		free(direccionFinal);
	}
	tablaEspecifica->cantTemps = 0;
	pthread_mutex_unlock(&tablaEspecifica->renombreEnCurso);
	while(NULL != (tdp = readdir(tableDirectory)))
	{
		if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, "..") || string_ends_with(tdp->d_name, ".cfg")){}
		else
		{
			bool firstRead = true;
			if(string_ends_with(tdp->d_name, ".tmpc"))
			{
				char* direccionTempC = malloc(strlen(direccionTabla) + strlen(tdp->d_name) + 1);
				strcpy(direccionTempC, direccionTabla);
				strcat(direccionTempC, tdp->d_name);
				int fullTempSize = obtenerTamanioArchivoConfig(direccionTempC);
				char** blocks = obtenerBloques(direccionTempC);
				int counter = 0;
				char* keysToParse = malloc(fullTempSize + 1);
				while(blocks[counter] != NULL)
				{
					char* blockContents = leerBloque(blocks[counter]);
					if(firstRead)
					{
						strcpy(keysToParse, blockContents);
						firstRead = false;
					}
					else
						strcat(keysToParse, blockContents);
					free(blockContents);
					counter++;
				}
				liberadorDeArrays(blocks);
				char** keyHandlerBeta = string_split(keysToParse, "\n");
				int recount = 0;
				while(keyHandlerBeta[recount] != NULL)
				{
					char* auxSend = malloc(strlen(keyHandlerBeta[recount]) + 3);
					strcpy(auxSend, keyHandlerBeta[recount]);
					strcat(auxSend, "\n");
					list_add(keysToManage, auxSend);
					recount++;
				}
				free(keysToParse);
				liberadorDeArrays(keyHandlerBeta);
				free(direccionTempC);
			}
			else
			{
				char* direccionPart = malloc(strlen(direccionTabla) + strlen(tdp->d_name) + 1);
				strcpy(direccionPart, direccionTabla);
				strcat(direccionPart, tdp->d_name);
				int fullTempSize = obtenerTamanioArchivoConfig(direccionPart);
				if(!fullTempSize){}
				else
				{
					char** blocks = obtenerBloques(direccionPart);
					int counter = 0;
					char* keysToParse = malloc(fullTempSize + 20);
					while(blocks[counter] != NULL)
					{
						char* blockContents = leerBloque(blocks[counter]);
						if(firstRead)
						{
							strcpy(keysToParse, blockContents);
							firstRead = false;
						}
						else
							strcat(keysToParse, blockContents);
						free(blockContents);
						counter++;
					}
					liberadorDeArrays(blocks);
					char** keyHandlerBeta = string_split(keysToParse, "\n");
					int recount = 0;
					while(keyHandlerBeta[recount] != NULL)
					{
						char* auxSend = malloc(strlen(keyHandlerBeta[recount]) + 3);
						strcpy(auxSend, keyHandlerBeta[recount]);
						strcat(auxSend, "\n");
						list_add(keysToManage, auxSend);
						recount++;
					}
					free(keysToParse);
					liberadorDeArrays(keyHandlerBeta);
				}
				free(direccionPart);
			}
		}
	}
	keysPostParsing = parsearKeys(keysToManage);
	char* direccionMetadata = malloc (strlen(direccionTabla) + 14);
	strcpy(direccionMetadata, direccionTabla);
	strcat(direccionMetadata, "Metadata.cfg");
	t_config* MetadataTabla = config_create(direccionMetadata);
	int particiones = config_get_int_value(MetadataTabla, "PARTICIONES");
	config_destroy(MetadataTabla);
	int g = 0;
	char* keysDeParticion;
	for(g = 0; g < particiones; g++)
	{
		keysDeParticion = obtenerKeysAPlasmar(keysPostParsing, g, particiones);
		if(NULL != keysDeParticion)
		{
			pthread_mutex_lock(&tablaEspecifica->compactacionActiva);
			char* auxg = string_itoa(g);
			char* direccionParticion = malloc(strlen(direccionTabla) + strlen(auxg) + 6);
			strcpy(direccionParticion, direccionTabla);
			strcat(direccionParticion, auxg);
			strcat(direccionParticion, ".bin");
			limpiarBloque(direccionParticion);
			t_config* particion = config_create(direccionParticion);
			char* sizedUse = string_itoa(strlen(keysDeParticion));
			config_set_value(particion, "SIZE", sizedUse);
			char* bloquesAsignados = escribirBloquesDeFs(keysDeParticion, strlen(keysDeParticion), tabla);
			config_set_value(particion, "BLOCKS", bloquesAsignados);
			config_save(particion);
			pthread_mutex_unlock(&tablaEspecifica->compactacionActiva);
			free(auxg);
			free(sizedUse);
			free(bloquesAsignados);
			config_destroy(particion);
			free(direccionParticion);
		}
		free(keysDeParticion);
	}
	struct dirent* tdp2;
	rewinddir(tableDirectory);
	pthread_mutex_lock(&tablaEspecifica->compactacionActiva);
	while(NULL != (tdp2 = readdir(tableDirectory)))
	{
		if(!strcmp(tdp2->d_name, ".") || !strcmp(tdp2->d_name, "..") || string_ends_with(tdp2->d_name, ".cfg")){}
		else
		{
			if(string_ends_with(tdp2->d_name, ".tmpc"))
			{
				char* direccionTempC = malloc(strlen(direccionTabla) + strlen(tdp2->d_name) + 1);
				strcpy(direccionTempC, direccionTabla);
				strcat(direccionTempC, tdp2->d_name);
				limpiarBloque(direccionTempC);
				remove(direccionTempC);
			}
		}
	}
	pthread_mutex_unlock(&tablaEspecifica->compactacionActiva);
	logInfo("[Compactador]: la %s ha sido compactada.", tabla);
	free(direccionMetadata);
	list_destroy_and_destroy_elements(keysPostParsing,(void*) &liberadorDeKeys);
	list_destroy_and_destroy_elements(keysToManage, &free);
	free(tdp);
	closedir(tableDirectory);
	free(direccionTabla);
}

char* obtenerKeysAPlasmar(t_list* keysPostParsing, int numeroDeParticion, int particiones)
{
	bool esDeTalParticion(t_keysetter* key)
	{
		bool result = (numeroDeParticion == key->key%particiones);
		return result;
	}

	t_list* keysAPlasmar = list_create();
	t_list* keysDeTalParticion;
	keysDeTalParticion = list_filter(keysPostParsing, (void*)esDeTalParticion);
	list_sort(keysDeTalParticion, (void*)chequearTimeKey);
	int a = 0;
	while(NULL != list_get(keysDeTalParticion, a))
	{
		bool perteneceALista(t_keysetter* key)
		{
			if(NULL != list_get(keysAPlasmar, 0))
			{
				t_keysetter* auxiliary = list_get(keysDeTalParticion, a);
				if(auxiliary->key == key->key)
					return auxiliary->key == key->key;
			}
			return false;
		}
		if(!list_find(keysAPlasmar, (void*)perteneceALista))
		{
			list_add(keysAPlasmar, list_get(keysDeTalParticion, a));
		}
		a++;
	}
	if(a != 0)
	{
		t_list* keysReParseadas;
		keysReParseadas = inversaParsearKeys(keysAPlasmar);
		int listIterator = 0;
		int sizeOfList = 0;
		while(NULL != list_get(keysReParseadas, listIterator))
		{
			sizeOfList += strlen(list_get(keysReParseadas, listIterator));
			listIterator ++;
		}
		listIterator = 0;
		char* allKeys = malloc(sizeOfList + 2);
		bool firstRead = true;
		while(NULL != list_get(keysReParseadas, listIterator))
		{
			if(firstRead)
			{
				strcpy(allKeys, list_get(keysReParseadas, listIterator));
				firstRead = false;
			}
			else
				strcat(allKeys, list_get(keysReParseadas, listIterator));
			listIterator++;
		}
		list_destroy_and_destroy_elements(keysReParseadas, &free);
		list_destroy(keysAPlasmar);
		list_destroy(keysDeTalParticion);
		return allKeys;
	}
	else
		return NULL;
}

void killProtocolCompactador()
{
	if(criticalFailure)
	{
		logError("[Compactador]: un error crítico ha sido decretado, el FS se desconectará sin dar tiempo a proteger datos.");
		list_clean_and_destroy_elements(tablasEnEjecucion, &free);
		free(TableEntry);
		closedir(directorioDeTablas);
		free(direccionDeLasTablas);
	}
	else
	{
		lastDumpSituation = 1;
		list_clean_and_destroy_elements(tablasEnEjecucion, &free);
		logInfo("[Compactador]: Desconectando todas las tablas.");
		usleep(slowestCompactationInterval * 1000); //Esto está para que el compactador tenga tiempo a matar todas las tablas de su sistema.
		free(TableEntry);
		closedir(directorioDeTablas);
		free(direccionDeLasTablas);
	}
}

