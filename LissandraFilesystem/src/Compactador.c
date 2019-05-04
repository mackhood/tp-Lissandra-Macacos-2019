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

	bool estaTabla(void* tablaDeLista)
	{
		char* tablaAux = malloc(strlen(tabla) + 1);
		strcpy(tablaAux, tabla);
		int cantCarac = strlen((char*)tablaDeLista);
		char* tablaDeListaAux;
		//char* tablaDeListaAux = string_new();
		tablaDeListaAux = malloc(cantCarac + 1);
		memcpy(tablaDeListaAux, tablaDeLista, cantCarac);
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
		if(!list_find(tablasEnEjecucion, estaTabla))
		{
			break;
		}
		else
		{
			//llamar compactacion
		}
	}
}


