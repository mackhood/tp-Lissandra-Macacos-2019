#include "Compactador.h"

void mainCompactador()
{
	//acceso a las tablas
	DIR* directorioDeTablas;
	struct dirent* tdp;
	char* auxdir = string_new();
	auxdir = malloc(strlen(punto_montaje) + 8);//le sumo 8 por tables/ y 1 por las dudas(siempre asignar uno de mas)
	strcpy(auxdir, punto_montaje);
	strcat(auxdir, "Tables/");
	if(NULL == (directorioDeTablas = opendir(auxdir)))
	{
		log_error(loggerLFL, "FileSystem: error al acceder al directorio de tablas, abortando");
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
}

void gestionarTabla(char*tabla)
{
	pthread_t hiloTabla;
	log_info(loggerLFL,"Compactador: Se inicio un hilo para manejar a %s.",tabla);
	pthread_create(&hiloTabla, NULL, (void *) compactarTablas, tabla);
	pthread_detach(hiloTabla);
}

void compactarTablas(char*tabla)
{

	bool estaTabla(char* tablaDeLista)
	{
		char* tablaAux = malloc(strlen(tabla) + 1);
		strcpy(tablaAux, tabla);
		int cantCarac = strlen(tablaDeLista);
		char* tablaDeListaAux = string_new();
		//char* tablaDeListaAux = string_new();
		tablaDeListaAux = malloc(cantCarac + 1);
		strcpy(tablaDeListaAux, tablaDeLista);
		bool result = (0 == strcmp(tablaDeListaAux, tablaAux));
		return result;
	}

	list_add(tablasEnEjecucion, tabla);
	char* direccionMetadataTabla= string_new();
	direccionMetadataTabla= malloc(strlen(punto_montaje) + strlen(tabla) + 21);//son 20 de tables/ y metadata.cfg +1 por las dudas
	strcpy(direccionMetadataTabla, punto_montaje);
	strcat(direccionMetadataTabla, "Tables/");
	strcat(direccionMetadataTabla, tabla);
	strcat(direccionMetadataTabla, "/Metadata.cfg");
	t_config * temporalArchivoConfig;
	temporalArchivoConfig = config_create(direccionMetadataTabla);
	int tiempoEntreCompactacion = config_get_int_value(temporalArchivoConfig, "TIEMPOENTRECOMPACTACIONES");
	while(1)
	{
		usleep(tiempoEntreCompactacion * 1000);
		if(!list_find(tablasEnEjecucion, (void*) estaTabla))
		{
			log_info(loggerLFL, "Compactador: La %s, al haber sido previamente desalojada dejará de ser buscada en el compactador");
			break;
		}
		else
		{
			//llamar compactacion
		}
	}
}

void gestionarDumps()
{
	pthread_t hiloMemtable;
	log_info(loggerLFL,"Compactador: Se inicio un hilo para manejar a la memtable");
	pthread_create(&hiloMemtable, NULL, (void *) gestionarMemtable, NULL);
	pthread_detach(hiloMemtable);
}

void gestionarMemtable()
{
	while(1)
	{
		usleep(tiempoDump * 1000);
		int a = 0;
		while(NULL != list_get(tablasEnEjecucion, a))
		{
			char* tabla = list_get(tablasEnEjecucion, a);
			crearTemporal(tabla);
			a++;
		}
		list_destroy(memtable);
		memtable = list_create();
	}
}

void crearTemporal(char* tabla)
{
	int perteneceATabla(t_Memtablekeys* key)
	{
		char* testTable = string_new();
		testTable = malloc(strlen(tabla) + 1);
		strcpy(testTable, tabla);
		return 0 == strcmp(key->tabla, testTable);
	}

	t_list* keysTableSpecific = list_create();
	keysTableSpecific = list_filter(memtable, (void*)perteneceATabla);
	t_Memtablekeys* auxiliaryKey;
	int a = 0;
	FILE* tempPointer;
	while(NULL != list_get(keysTableSpecific, a))
	{
		auxiliaryKey = malloc(sizeof(t_Memtablekeys) + 4);
		auxiliaryKey = list_get(keysTableSpecific, a);
		int sizeOfKey = sizeof(uint16_t) + strlen(auxiliaryKey->data->clave) + sizeof(double) + 1;
		char* claveParaTemp = malloc(sizeOfKey + 1);
	}
}


