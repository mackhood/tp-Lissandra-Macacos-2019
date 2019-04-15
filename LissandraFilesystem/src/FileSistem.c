#include "FileSistem.h"

void mainFileSistem()
{

}

void setearValoresFileSistem(t_config * archivoConfig)
{
	punto_montaje = config_get_string_value(archivoConfig, "PUNTO_MONTAJE");
}

void crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	/*DIR* newdir;
	char buff[128];
	memset(buff,0,sizeof(buff));
	strcpy(buff, punto_montaje);

	if(NULL == (newdir = opendir(punto_montaje)))// reviso si el punot de montaje es accesible
	{
		log_info(loggerLFL,"FileSistem: El directorio que usted desea crear no es accesible");
		exit(1);
	}
	else
	{
		char* direccionFinal;
		if(buff[strlen(buff-1)]=='/') //reviso si me pusieron una barra al final de la dirección
		{
			char* nombreconbarra = malloc(strlen(nombreconbarra));
			nombreconbarra = strcat(nombre,"/");
			strncpy(buff + strlen(buff),nombreconbarra,7);
			strcpy(direccionFinal, nombreconbarra);
		}
		else
		{
			char* nombreconbarra = malloc(strlen(nombreconbarra));
			nombreconbarra = strcat(nombre,"/");
			char* nombrecondosbarras = malloc(strlen(nombrecondosbarras));
			nombrecondosbarras = strcat("/",nombreconbarra);
			strncpy(buff + strlen(buff),nombrecondosbarras,8);
			strcpy(direccionFinal, nombrecondosbarras);
		}
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
			int resultsb = crearParticiones(direccionFinal, particiones); //Crea archivos .bin
			if(resultsb == 1)
			{
				log_info(loggerLFL, "FileSistem: Error al crear tabla, abortando");
				closedir(newdir);
				exit(1);
			}
			else
				closedir(newdir);
		}
	}
	free(buff);
	free(newdir);*/
}

int crearMetadata (char* direccion, char* consistencia, int particiones, int tiempoCompactacion)
{
	/*char* direccionDelMetadata;
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
		char* Linea = malloc(15);
		Linea = strcat("Consistencia=",consistencia"\n");
		fwrite(Linea, strlen(Linea), 1, metadata);
		free(Linea);
	}*/
}

int crearParticiones(char* direccionFinal, int particiones)
{

}

