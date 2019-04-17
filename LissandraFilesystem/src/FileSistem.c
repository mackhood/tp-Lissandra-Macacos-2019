#include "FileSistem.h"

void mainFileSistem()
{

}

void setearValoresFileSistem(t_config * archivoConfig)
{
	punto_montaje = config_get_string_value(archivoConfig, "PUNTO_MONTAJE");
	crearTabla("TablaA", "SC","5", "6000");
}

void crearTabla(char* nombre, char* consistencia, char* particiones, char* tiempoCompactacion)
{
	DIR* newdir;
	char buff[128];
	char* tablename = string_new();
	strcpy(tablename, nombre);
	memset(buff,0,sizeof(buff));
	strcpy(buff, punto_montaje);

	if(NULL == (newdir = opendir(punto_montaje)))// reviso si el punto de montaje es accesible
	{
		log_info(loggerLFL,"FileSistem: El directorio que usted desea crear no es accesible");
		exit(1);
	}
	else
	{
		char* direccionFinal = string_new();
		strcat(tablename,"/");
		strncpy(buff + strlen(buff),tablename,strlen(tablename));
		strncpy(direccionFinal, tablename, strlen(tablename));

		log_info(loggerLFL, "FileSistem: Se procede a la creacion del directorio");
		mkdir(buff, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //creo el directorio de la tabla con sus respectivos permisos
		int results = crearMetadata(direccionFinal, consistencia, particiones, tiempoCompactacion); //Crea el archivio metadata
		if (results == 1)
		{
			log_info(loggerLFL, "FileSistem: Error al crear tabla, abortando");
			closedir(newdir);
			exit(1);
		}
		else
		{
			//int resultsb = crearParticiones(direccionFinal, particiones); //Crea archivos .bin
			//if(resultsb == 1)
			//{
			//	log_info(loggerLFL, "FileSistem: Error al crear tabla, abortando");
			//	closedir(newdir);
			//	exit(1);
			//}
			//else
				closedir(newdir);
		}
		free(direccionFinal);
	}
	free(newdir);
}

int crearMetadata (char* direccion, char* consistencia, char* particiones, char* tiempoCompactacion)
{
	char* direccionDelMetadata;
	FILE* metadata;
	direccionDelMetadata = strcat(direccion, "Metadata.bin");
	metadata = fopen(direccionDelMetadata, "wb+");
	if(metadata == NULL)
	{
		log_info(loggerLFL,"FileSistem: No se pudo crear el archivo Metadata");
		return 1;
	}
	else
	{
		char* Linea = malloc(17);
		Linea = strcat("CONSISTENCIA=",consistencia);
		strcat(Linea, "\n");
		fwrite(Linea, strlen(Linea), 1, metadata);
		free(Linea);
		return 0;
		/*char* Linea2 = malloc(17);
		char * cant_particion;
		Linea2 = strcat("PARTICIONES=",particiones);
		strcat(Linea2, "\n");
		fwrite(Linea2, strlen(Linea2), 1, metadata);
		free(Linea2);
		char* Linea3 = malloc(17);
		Linea3 = strcat("Consistencia=",consistencia);
		strcat(Linea3, "\n");
		fwrite(Linea3, strlen(Linea3), 1, metadata);
		free(Linea3);*/
	}
}

/*int crearParticiones(char* direccionFinal, char* particiones)
{

}*/

