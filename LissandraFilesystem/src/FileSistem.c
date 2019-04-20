#include "FileSistem.h"

void mainFileSistem()
{

}

void setearValoresFileSistem(t_config * archivoConfig)
{
	punto_montaje = strdup(config_get_string_value(archivoConfig, "PUNTO_MONTAJE"));
	strcat(punto_montaje, "\0");
	//crearTabla("TABLAA", "SC", 5, 6000);
}

int crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	DIR* checkdir;
	char* checkaux = string_new();
	checkaux = malloc(strlen(nombre) + strlen(punto_montaje) + 8);
	DIR* newdir;
	char buff[128];
	char* tablename = string_new();
	tablename = malloc(strlen(nombre)+1);
	char* puntodemontaje = string_new();
	puntodemontaje = malloc(strlen(nombre) + strlen(punto_montaje) + 8);
	strcpy(puntodemontaje, punto_montaje);
	strcpy(tablename, nombre);
	memset(buff, 0, sizeof(buff));
	strcat(puntodemontaje, "Tables/");
	strcpy(buff, puntodemontaje);
	strcpy(checkaux, puntodemontaje);
	strcat(checkaux, nombre);
	if(NULL == (newdir = opendir(punto_montaje)))// reviso si el punto de montaje es accesible
	{
		perror("[ERROR] Punto de montaje no accesible");
		log_error(loggerLFL,"FileSistem: El punto de montaje al que usted desea entrar no es accesible");
		return(1);
	}

	else
	{
		if(NULL != (checkdir = opendir(checkaux)))
		{
			log_info(loggerLFL, "FileSystem: La tabla que usted quiere crear ya existe");
			return(2);
		}
		else
		{
			char* direccionFinal = string_new();
			strcat(tablename,"/");
			strncpy(buff + strlen(buff), tablename, strlen(tablename));
			strcat(puntodemontaje, tablename);
			direccionFinal = malloc(strlen(puntodemontaje) + 1);
			strcpy(direccionFinal, puntodemontaje);

			log_info(loggerLFL, "FileSistem: Se procede a la creacion del directorio");
			mkdir(buff, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //creo el directorio de la tabla con sus respectivos permisos
			int results = crearMetadata(direccionFinal, consistencia, particiones, tiempoCompactacion); //Crea el archivio metadata
			if (results == 1)
			{
				perror("[ERROR] Error al crear Metadata, abortando");
				log_error(loggerLFL, "FileSistem: Error al crear Metadata, abortando");
				closedir(newdir);
				return(1);
			}
			else
			{
				int resultsb = crearParticiones(direccionFinal, particiones); //Crea archivos .bin
				if(resultsb == 1)
				{
					perror("[ERROR] Error al crear particiones, abortando");
					log_error(loggerLFL, "FileSistem: Error al crear particiones, abortando");
					closedir(newdir);
					return(1);
				}
				else
					closedir(newdir);
			}
			free(direccionFinal);
			log_info(loggerLFL, "FileSystem: Tabla creada satisfactoriamente");
			return(0);
		}
	}
}

int crearMetadata (char* direccion, char* consistencia, int particiones, int tiempoCompactacion)
{
	char* direccionaux = string_new();
	direccionaux = malloc(strlen(direccion)+13);
	strcpy(direccionaux, direccion);
	char* direccionDelMetadata = malloc(strlen(direccionaux)+13);
	FILE* metadata;
	strcat(direccionaux, "Metadata.cfg");
	strcpy(direccionDelMetadata, direccionaux);
	metadata = fopen(direccionDelMetadata, "w+");
	if(metadata == NULL)
	{
		return 1;
	}
	else
	{
		char* Linea = string_new();
		Linea = malloc(17);
		strcpy(Linea, "CONSISTENCIA=");
		strcat(Linea, consistencia);
		strcat(Linea,"\n");
		fwrite(Linea, strlen(Linea), 1, metadata);
		free(Linea);
		char* Linea2 = string_new();
		char* cantparticiones = string_new();
		Linea2 = malloc(sizeof(particiones) + 14);
		strcpy(Linea2, "PARTICIONES=");
		cantparticiones = string_itoa(particiones);
		strcat(Linea2, cantparticiones);
		strcat(Linea2, "\n");
		fwrite(Linea2, strlen(Linea2), 1, metadata);
		free(Linea2);
		char* Linea3 = string_new();
		char* tiempoEntreCompactaciones = string_new();
		Linea3 = malloc(sizeof(tiempoCompactacion) + 28);
		strcpy(Linea3, "TIEMPOENTRECOMPACTACIONES=");
		tiempoEntreCompactaciones = string_itoa(tiempoCompactacion);
		strcat(Linea3, tiempoEntreCompactaciones);
		strcat(Linea3, "\n");
		fwrite(Linea3, strlen(Linea3), 1, metadata);
		fclose(metadata);
		return 0;
	}
}

int crearParticiones(char* direccionFinal, int particiones)
{
	int i = 0;
	FILE* particion;
	while(i < particiones)
	{
		char* particionado = string_new();
		char* aux = string_new();
		aux = malloc(sizeof(i)+1);
		particionado = malloc(strlen(direccionFinal)+sizeof(i)+4);
		strcpy(aux, string_itoa(i));
		strcpy(particionado, direccionFinal);
		strcat(particionado, aux);
		strcat(particionado, ".bin");
		particion = fopen(particionado, "w+");
		if(particion == NULL)
			return 1;
		else
		{
			char* size = string_new();
			size = malloc(7);
			strcpy(size, "SIZE=");
			strcat(size, "\n");
			fwrite(size, strlen(size), 1, particion);
			char* blocks = string_new();
			blocks = malloc (9);
			strcpy(blocks, "BLOCKS=");
			strcat(blocks, "\n");
			fwrite(blocks, strlen(blocks), 1, particion);
			free(particionado);
			fclose(particion);
		}
		i++;
	}
	return 0;
}

