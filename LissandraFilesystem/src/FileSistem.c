#include "FileSistem.h"

void mainFileSistem()
{
	testerFileSystem();
}

void testerFileSystem()
{
	creatingFL = 1;
	char* direccionFileSystem = string_new();
	direccionFileSystem = malloc(strlen(punto_montaje) + 8);
	strcpy(direccionFileSystem, punto_montaje);
	strcat(direccionFileSystem, "Blocks/");
	char* aux = string_new();
	aux = malloc(strlen(direccionFileSystem) + 8);
	strcpy(aux, direccionFileSystem);
	strcat(aux, "0.bin");
	FILE* doomsdaypointer;
	if(NULL == (doomsdaypointer = fopen(aux, "r")))
	{
		log_info(loggerLFL, "FileSystem: Se procede a crear los bloques de memoria");
		crearParticiones(direccionFileSystem, blocks);
	}
	levantarBitmap(direccionFileSystem);
	free(direccionFileSystem);
	free(aux);
}

void levantarBitmap(char* direccion)
{
	int i;
	log_info(loggerLFL, "FileSystem: Se procede a crear el bitmap");
	char* direccionBitmap= string_new();
	direccionBitmap= malloc(strlen(punto_montaje) + 21);
	strcpy(direccionBitmap, punto_montaje);
	strcat(direccionBitmap, "Metadata/Bitmap.bin");
	FILE* bitmap = fopen(direccionBitmap, "w+");
	for(i = 0; i < blocks; i++)
	{
		char* direccionpuenteada = string_new();
		direccionpuenteada = malloc(strlen(direccion) + 10);
		strcpy(direccionpuenteada, direccion);
		char* aux = string_new();
		aux = malloc(sizeof(i) + 1);
		strcpy(aux, string_itoa(i));
		strcat(direccionpuenteada, aux);
		strcat(direccionpuenteada, ".bin");
		FILE* blockpointer = fopen(direccionpuenteada, "r");
		if(blockpointer == NULL)
		{
			log_error(loggerLFL, "FileSystem: No se encuentran los bloques del File System");
			fclose(blockpointer);
			free(direccionpuenteada);
			free(aux);
			break;
		}
		else
		{
			char* a = string_new();
			a = malloc(64);
			fread(a, 64, 1, blockpointer);
			int auxcomp = 0;
			auxcomp = strcmp(a, "");
			if(auxcomp == 0)
			{
				int auxb = 0;
				fwrite(&auxb, 4, 1, bitmap);
			}
			else
			{
				int auxb = 1;
				fwrite(&auxb, 4, 1, bitmap);
			}
			free(a);
		}
		free(aux);
		free(direccionpuenteada);
		fclose(blockpointer);
	}
	fclose(bitmap);
	free(direccionBitmap);
	log_info(loggerLFL, "FileSystem: El bitmap fue creado satisfactoriamente");
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
	free(direccionMetadataFileSystem);
	config_destroy(temporalArchivoConfig);
}

int crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	creatingFL = 0;
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
	if(NULL == (newdir = opendir(punto_montaje)))// reviso si el punto de montaje es accesible
	{
		perror("[ERROR] Punto de montaje no accesible");
		log_error(loggerLFL,"FileSistem: El punto de montaje al que usted desea entrar no es accesible");
		return(1);
	}

	else
	{
		if(1 == existeTabla(nombre))
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
		particionado = malloc(strlen(direccionFinal)+sizeof(i)+5);
		strcpy(aux, string_itoa(i));
		strcpy(particionado, direccionFinal);
		strcat(particionado, aux);
		strcat(particionado, ".bin");
		particion = fopen(particionado, "w+");
		if(particion == NULL)
			return 1;
		else
		{
			if(creatingFL == 0)
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
			else
			{
				free(particionado);
				fclose(particion);
			}
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

int existeTabla(char* tabla)
{
	DIR* checkdir;
	char* checkaux = string_new();
	checkaux = malloc(strlen(tabla) + strlen(punto_montaje) + 9);
	char* tablename = string_new();
	tablename = malloc(strlen(tabla) + 3);
	char* puntodemontaje = string_new();
	puntodemontaje = malloc(strlen(tabla) + strlen(punto_montaje) + 9);
	strcpy(puntodemontaje, punto_montaje);
	strcpy(tablename, tabla);
	strcat(puntodemontaje, "Tables/");
	strcpy(checkaux, puntodemontaje);
	strcat(checkaux, tabla);
	if(NULL != (checkdir = opendir(checkaux)))
		return 1;
	else
		return 0;
}

//void mostrarMetadataEspecificado(char* tabla, bool solicitadoPorMemoria, void* buffer)
//{
//	if(0 == existeTabla)
//	{
//		log_info(loggerLFL, "FileSystem: La tabla a la que quiere acceder no existe");
//
//	}
//	else
//	{
//		if(solicitadoPorMemoria)
//		{
//
//		}
//		else
//		{
//
//		}
//	}
//}
//
//char* mostrarTodosLosMetadatas(bool solicitadoPorMemoria,, void* buffer)
//{
//
//}

