#include "FileSistem.h"

void mainFileSistem()
{
//	testerFileSystem();
}

void testerFileSystem()
{
	char* direccionFileSystem = string_new();
	direccionFileSystem = malloc(strlen(punto_montaje) + 8);
	strcpy(direccionFileSystem, punto_montaje);
	strcat(direccionFileSystem, "Blocks/");
	DIR* checkdir;
	checkdir = opendir(direccionFileSystem);
	if(NULL == readdir(checkdir))
		crearParticiones(direccionFileSystem, blocks);

}

void setearValoresFileSistem(t_config * archivoConfig)
{
	punto_montaje = strdup(config_get_string_value(archivoConfig, "PUNTO_MONTAJE"));
	char* direccionMetadataFileSystem = string_new();
	direccionMetadataFileSystem = malloc(strlen(punto_montaje) + 23);
	strcpy(direccionMetadataFileSystem, punto_montaje);
	strcat(direccionMetadataFileSystem, "Metadata/Metadata.cfg");
	t_config * temporalArchivoConfig;
	temporalArchivoConfig = config_create(direccionMetadataFileSystem);
	blocks = config_get_int_value(temporalArchivoConfig, "BLOCKS");
	tamanio_bloques = config_get_int_value(temporalArchivoConfig, "BLOCK_SIZE");

}

int crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	DIR* checkdir;
	char* checkaux = string_new();
	checkaux = malloc(strlen(nombre) + strlen(punto_montaje) + 9);
	DIR* newdir;
	char buff[128];
	char* tablename = string_new();
	tablename = malloc(strlen(nombre) + 3);
	char* puntodemontaje = string_new();
	puntodemontaje = malloc(strlen(nombre) + strlen(punto_montaje) + 9);
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
			strncpy(buff + strlen(buff), tablename, strlen(tablename) + 1);
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

int dropTable(char* tablaPorEliminar)
{
	DIR* checkdir;
	DIR* newdir;
	char* checkaux = string_new();
	char* tablename = string_new();
	char* puntodemontaje = string_new();
	tablename = malloc(strlen(tablaPorEliminar) + 2);
	checkaux = malloc(strlen(tablaPorEliminar) + strlen(punto_montaje) + 9);
	puntodemontaje = malloc(strlen(tablaPorEliminar) + strlen(punto_montaje) + 9);
	strcpy(puntodemontaje, punto_montaje);
	strcat(puntodemontaje, "Tables/");
	strcpy(tablename, tablaPorEliminar);
	strcpy(checkaux, puntodemontaje);
	strcat(checkaux, tablaPorEliminar);
	if(NULL == (newdir = opendir(punto_montaje)))// reviso si el punto de montaje es accesible
	{
		perror("[ERROR] Punto de montaje no accesible");
		log_error(loggerLFL,"FileSistem: El punto de montaje al que usted desea entrar no es accesible");
		closedir(newdir);
		return(1);
	}
	else
	{
		if(NULL == (checkdir = opendir(checkaux)))
		{
			log_info(loggerLFL, "FileSystem: La tabla que usted quiere borrar no existe");
			return(2);
		}
		else
		{
			closedir(checkdir);
			if (limpiadorDeArchivos(checkaux) == 1)
			{
				rmdir(checkaux);
				log_info(loggerLFL, "FileSystem: el directorio ha sido eliminado correctamente");
				return 0;
			}
			else
			{
				log_error(loggerLFL, "FileSystem: error al eliminar el directorio");
				return(3);
			}
		}
	}
	free(tablename);
	free(checkaux);
	free(puntodemontaje);
}

int limpiadorDeArchivos(char* direccion)
{
	char* direccionMetadata = string_new();
	direccionMetadata = malloc(strlen(direccion) + 14);
	strcpy(direccionMetadata, direccion);
	strcat(direccionMetadata, "/Metadata.cfg");
	t_config * temporalArchivoConfig;
	temporalArchivoConfig = config_create(direccionMetadata);
	int particiones = 0;
	particiones = config_get_int_value(temporalArchivoConfig, "PARTICIONES");
	int i;
	for(i = 0; i < particiones; i++)
	{
		char* direccionBin = string_new();
		direccionBin = malloc(strlen(direccion) + sizeof(i) + 3);
		strcpy(direccionBin, direccion);
		char* archivo = string_new();
		archivo = malloc(sizeof(i) + 2);
		strcpy(archivo, string_itoa(i));
		strcat(archivo, ".bin");
		strcat(direccionBin, "/");
		strcat(direccionBin, archivo);
		int status = remove(direccionBin);
		if(status == 0)
			continue;
		else
		{
			log_error(loggerLFL, "FileSystem: Error al eliminar particiones");
			return 0;
		}
		free(direccionBin);
		free(archivo);
	}
	int statusMeta = remove(direccionMetadata);
	if(statusMeta == 0)
	{
		free(direccionMetadata);
		config_destroy(temporalArchivoConfig);
		return 1;
	}
	else
	{
		free(direccionMetadata);
		config_destroy(temporalArchivoConfig);
		return 0;
	}
}

