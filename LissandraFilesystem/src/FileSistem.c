#include "FileSistem.h"

void mainFileSistem()
{
	testerFileSystem();
}

void testerFileSystem()
{
	creatingFL = 1;
	char* direccionFileSystem = malloc(strlen(punto_montaje) + 8);
	strcpy(direccionFileSystem, punto_montaje);
	strcat(direccionFileSystem, "Blocks/");
	direccionFileSystemBlocks = malloc(strlen(direccionFileSystem) + 1);
	strcpy(direccionFileSystemBlocks, direccionFileSystem);
	char* aux = malloc(strlen(direccionFileSystem) + 8);
	strcpy(aux, direccionFileSystem);
	strcat(aux, "0.bin");
	FILE* doomsdaypointer;
	if(NULL == (doomsdaypointer = fopen(aux, "r+")))
	{
		logInfo( "FileSystem: Se procede a crear los bloques de memoria");
		crearParticiones(direccionFileSystem, blocks);
	}
	else
		fclose(doomsdaypointer);
	levantarBitmap(direccionFileSystem);
	free(direccionFileSystem);
	free(aux);
}

void levantarBitmap(char* direccion)
{
	logInfo( "FileSystem: Se procede a crear el bitmap");
	char* direccionBitmap = malloc(strlen(punto_montaje) + 21);
	strcpy(direccionBitmap, punto_montaje);
	strcat(direccionBitmap, "Metadata/Bitmap.bin");
	globalBitmapPath = malloc(strlen(direccionBitmap) + 1);
	strcpy(globalBitmapPath, direccionBitmap);
    bitarrayfd = open(globalBitmapPath, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(bitarrayfd, (blocks/8));
    if (bitarrayfd == -1)
    {
    	logError("FileSystem: error al abrir el bitmap, abortando sistema");
    	free(bitarraycontent);
    	close(bitarrayfd);
    	signalExit = true;
    }
    else
    {
    	bitarraycontent = mmap(NULL, (blocks/8), PROT_READ | PROT_WRITE, MAP_SHARED, bitarrayfd, 0);
    	bitarray = bitarray_create_with_mode(bitarraycontent, (blocks/8), LSB_FIRST);
    	int a;
    	for(a = 0; a < blocks; a++)
    	{
    		char* auxb = string_itoa(a);
    		char* direccionBloque = malloc(strlen(direccionFileSystemBlocks) + strlen(auxb) + 5);
    		strcpy(direccionBloque, direccionFileSystemBlocks);
    		strcat(direccionBloque, auxb);
    		strcat(direccionBloque, ".bin");
    		FILE* blockpointer = fopen(direccionBloque, "r+");
    		fseek(blockpointer, 0, SEEK_END);
    		unsigned long blocklength = (unsigned long)ftell(blockpointer);
    		if(0 == blocklength)
    		{
    			bitarray_clean_bit(bitarray, a);
    		}
    		else
    		{
        		bitarray_set_bit(bitarray, a);
    		}
    		free(auxb);
    		fclose(blockpointer);
    		free(direccionBloque);
    	}
		msync(bitarraycontent, bitarrayfd, MS_SYNC);
		free(direccionBitmap);
    }
}

void setearValoresFileSistem(t_config * archivoConfig)
{
	fileSystemFull = false;
	punto_montaje = strdup(config_get_string_value(archivoConfig, "PUNTO_MONTAJE"));
	char* direccionMetadataFileSystem = malloc(strlen(punto_montaje) + 23);
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
	if(particiones > cantidadDeBloquesLibres())
	{
		logError("FileSystem: No hay suficientes bloques para asignar a la tabla.");
		return 5;
	}
	creatingFL = 0;
	DIR* newdir;
	char buff[128];
	char* tablename = malloc(strlen(nombre) + 3);
	char* puntodemontaje = malloc(strlen(nombre) + strlen(punto_montaje) + 9);
	strcpy(puntodemontaje, punto_montaje);
	strcpy(tablename, nombre);
	memset(buff, 0, sizeof(buff));
	strcat(puntodemontaje, "Tables/");
	strcpy(buff, puntodemontaje);
	if(NULL == (newdir = opendir(punto_montaje)))// reviso si el punto de montaje es accesible
	{
		perror("[ERROR] Punto de montaje no accesible");
		logError("FileSistem: El punto de montaje al que usted desea entrar no es accesible");
		free(tablename);
		free(puntodemontaje);
		return(1);
	}

	else
	{
		if(1 == existeTabla(nombre))
		{
			logInfo( "FileSystem: La tabla que usted quiere crear ya existe");
			free(tablename);
			free(puntodemontaje);
			return(2);
		}
		else
		{
			strcat(tablename,"/");
			strncpy(buff + strlen(buff), tablename, strlen(tablename) + 1);
			strcat(puntodemontaje, tablename);
			char* direccionFinal = malloc(strlen(puntodemontaje) + 1);
			strcpy(direccionFinal, puntodemontaje);

			logInfo( "FileSistem: Se procede a la creacion del directorio");
			mkdir(buff, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //creo el directorio de la tabla con sus respectivos permisos
			int results = crearMetadata(direccionFinal, consistencia, particiones, tiempoCompactacion); //Crea el archivio metadata
			if (results == 1)
			{
				perror("[ERROR] Error al crear Metadata, abortando");
				logError( "FileSistem: Error al crear Metadata, abortando");
				closedir(newdir);
				free(tablename);
				free(puntodemontaje);
				return(1);
			}
			else
			{
				int resultsb = crearParticiones(direccionFinal, particiones); //Crea archivos .bin
				if(resultsb == 1)
				{
					perror("[ERROR] Error al crear particiones, abortando");
					logError( "FileSistem: Error al crear particiones, abortando");
					closedir(newdir);
					free(tablename);
					free(puntodemontaje);
					return(1);
				}
				else
				{
					closedir(newdir);
				}
			}
			free(direccionFinal);
			logInfo("FileSystem: Tabla creada satisfactoriamente");
			free(tablename);
			free(puntodemontaje);
			return(0);
		}
	}
}

int crearMetadata (char* direccion, char* consistencia, int particiones, int tiempoCompactacion)
{
	char* direccionaux = malloc(strlen(direccion)+13);
	strcpy(direccionaux, direccion);
	char* direccionDelMetadata = malloc(strlen(direccionaux)+13);
	FILE* metadata;
	strcat(direccionaux, "Metadata.cfg");
	strcpy(direccionDelMetadata, direccionaux);
	free(direccionaux);
	metadata = fopen(direccionDelMetadata, "w+");
	if(metadata == NULL)
	{
		free(direccionDelMetadata);
		return 1;
	}
	else
	{
		char* Linea = malloc(18);
		strcpy(Linea, "CONSISTENCIA=");
		strcat(Linea, consistencia);
		strcat(Linea,"\n");
		fwrite(Linea, strlen(Linea), 1, metadata);
		free(Linea);
		char* Linea2 = malloc(sizeof(particiones) + 14);
		char* cantparticiones = string_itoa(particiones);
		strcpy(Linea2, "PARTICIONES=");
		strcat(Linea2, cantparticiones);
		strcat(Linea2, "\n");
		fwrite(Linea2, strlen(Linea2), 1, metadata);
		free(Linea2);
		char* Linea3 = malloc(strlen(string_itoa(tiempoCompactacion)) + 28);
		char* tiempoEntreCompactaciones = string_itoa(tiempoCompactacion);
		strcpy(Linea3, "TIEMPOENTRECOMPACTACIONES=");
		strcat(Linea3, tiempoEntreCompactaciones);
		strcat(Linea3, "\n");
		fwrite(Linea3, strlen(Linea3), 1, metadata);
		fclose(metadata);
		free(direccionDelMetadata);
		return 0;
	}
}

int crearParticiones(char* direccionFinal, int particiones)
{
	int i = 0;
	FILE* particion;
	while(i < particiones)
	{
		char* aux = malloc(sizeof(i)+1);
		char* particionado = malloc(strlen(direccionFinal)+sizeof(i)+5);
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
				char* size = malloc(8);
				strcpy(size, "SIZE=0");
				strcat(size, "\n");
				fwrite(size, strlen(size), 1, particion);
				char* blocks = malloc(sizeof(int) + 13);
				char* auxBlock = obtenerBloqueLibre();
				int blocksUsed = 0;
				int seizedSize = 0;
				escribirBloque(&blocksUsed, &seizedSize, 1, auxBlock, " ");
				strcpy(blocks, "BLOCKS=[");
				strcat(blocks, auxBlock);
				strcat(blocks, "]\n");
				fwrite(blocks, strlen(blocks), 1, particion);
				free(particionado);
				fclose(particion);
				free(auxBlock);
				free(size);
				free(blocks);
			}
			else
			{
				free(particionado);
				fclose(particion);
			}
		}
		free(aux);
		i++;
	}
	return 0;
}

int dropTable(char* tablaPorEliminar)
{
	DIR* checkdir;
	DIR* newdir;
	char* tablename = malloc(strlen(tablaPorEliminar) + 2);
	char* checkaux = malloc(strlen(tablaPorEliminar) + strlen(punto_montaje) + 9);
	char* puntodemontaje = malloc(strlen(tablaPorEliminar) + strlen(punto_montaje) + 9);
	strcpy(puntodemontaje, punto_montaje);
	strcat(puntodemontaje, "Tables/");
	strcpy(tablename, tablaPorEliminar);
	strcpy(checkaux, puntodemontaje);
	strcat(checkaux, tablaPorEliminar);
	newdir = opendir(punto_montaje);
	if(NULL == newdir)// reviso si el punto de montaje es accesible
	{
		perror("[ERROR] Punto de montaje no accesible");
		logError("FileSistem: El punto de montaje al que usted desea entrar no es accesible");
		closedir(newdir);
		free(tablename);
		free(checkaux);
		free(puntodemontaje);
		return(1);
	}
	else
	{
		if(NULL == (checkdir = opendir(checkaux)))
		{
			logInfo( "FileSystem: La tabla que usted quiere borrar no existe");
			closedir(newdir);
			free(tablename);
			free(checkaux);
			free(puntodemontaje);
			return(2);
		}
		else
		{
			closedir(newdir);
			closedir(checkdir);
			if (limpiadorDeArchivos(checkaux, tablaPorEliminar) == 1)
			{
				rmdir(checkaux);
				logInfo( "FileSystem: el directorio ha sido eliminado correctamente");
				free(tablename);
				free(checkaux);
				free(puntodemontaje);
				return 0;
			}
			else
			{
				logError( "FileSystem: error al eliminar el directorio");
				free(tablename);
				free(checkaux);
				free(puntodemontaje);
				return(3);
			}
		}
	}
}

int limpiadorDeArchivos(char* direccion, char* tabla)
{
	bool estaTabla(t_TablaEnEjecucion* tablaDeLista)
	{
		char* tablaAux = malloc(strlen(tabla) + 1);
		strcpy(tablaAux, tabla);
		int cantCarac = strlen(tablaDeLista->tabla);
		char* tablaDeListaAux = malloc(cantCarac + 1);
		strcpy(tablaDeListaAux, tablaDeLista->tabla);
		bool result = (0 == strcmp(tablaDeListaAux, tablaAux));
		free(tablaAux);
		free(tablaDeListaAux);
		return result;
	}
	limpiadorDeBloques(direccion);
	char* direccionMetadata = malloc(strlen(direccion) + 14);
	strcpy(direccionMetadata, direccion);
	strcat(direccionMetadata, "/Metadata.cfg");
	t_config * temporalArchivoConfig;
	temporalArchivoConfig = config_create(direccionMetadata);
	int particiones = 0;
	particiones = config_get_int_value(temporalArchivoConfig, "PARTICIONES");
	int i;
	for(i = 0; i < particiones; i++)
	{
		char* direccionBin = malloc(strlen(direccion) + sizeof(i) + 3);
		strcpy(direccionBin, direccion);
		char* archivo = malloc(sizeof(i) + 2);
		strcpy(archivo, string_itoa(i));
		strcat(archivo, ".bin");
		strcat(direccionBin, "/");
		strcat(direccionBin, archivo);
		int status = remove(direccionBin);
		if(status == 0){}
		else
		{
			logError( "FileSystem: Error al eliminar particiones");
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
	}
	else
	{
		free(direccionMetadata);
		config_destroy(temporalArchivoConfig);
		return 0;
	}
	t_TablaEnEjecucion* tablaABorrar = list_find(tablasEnEjecucion, (void*)estaTabla);
	int b;
	if(tablaABorrar->cantTemps == 0)
		return 1;
	for(b = 0; b < tablaABorrar->cantTemps; b++)
	{
		char* direccionTemp = malloc(strlen(direccion) + strlen(string_itoa(b)) + 6);
		strcpy(direccionTemp, direccion);
		char* archivo = malloc(strlen(string_itoa(b)) + 5);
		strcpy(archivo, string_itoa(b));
		strcat(archivo, ".tmp");
		strcat(direccionTemp, "/");
		strcat(direccionTemp, archivo);
		int statusTemp = remove(direccionTemp);
		if(statusTemp == 0){}
		else
		{
			logError( "FileSystem: Error al eliminar temporales");
			free(direccionTemp);
			free(archivo);
			return 0;
		}
		free(direccionTemp);
		free(archivo);
	}
	return 1;
}

int existeTabla(char* tabla)
{
	DIR* checkdir;
	char* checkaux = malloc(strlen(tabla) + strlen(punto_montaje) + 9);
	char* tablename = malloc(strlen(tabla) + 3);
	char* puntodemontaje = malloc(strlen(tabla) + strlen(punto_montaje) + 9);
	strcpy(puntodemontaje, punto_montaje);
	strcpy(tablename, tabla);
	strcat(puntodemontaje, "Tables/");
	strcpy(checkaux, puntodemontaje);
	strcat(checkaux, tabla);
	checkdir = opendir(checkaux);
	if(NULL != checkdir)
	{
		closedir(checkdir);
		free(checkaux);
		free(puntodemontaje);
		free(tablename);
		return 1;
	}
	else
	{
		closedir(checkdir);
		free(checkaux);
		free(puntodemontaje);
		free(tablename);
		return 0;
	}
}

char* mostrarMetadataEspecificada(char* tabla, bool solicitadoPorMemoria)
{
	if(0 == existeTabla(tabla))
	{
		logInfo( "FileSystem: La tabla a la que quiere acceder no existe");
		puts("La tabla a la que usted quiere acceder no existe.");
		return NULL;
	}
	else
	{
		char* auxdir = malloc(strlen(punto_montaje) + 8);
		strcpy(auxdir, punto_montaje);
		strcat(auxdir, "Tables/");
		char* direccionDeTableMetadata = malloc(strlen(auxdir) + strlen(tabla) + 15);
		strcpy(direccionDeTableMetadata, auxdir);
		strcat(direccionDeTableMetadata, tabla);
		strcat(direccionDeTableMetadata, "/Metadata.cfg");
		t_config * temporalArchivoConfig = config_create(direccionDeTableMetadata);
		char* consistencia = strdup(config_get_string_value(temporalArchivoConfig, "CONSISTENCIA"));
		int	cantParticiones = config_get_int_value(temporalArchivoConfig, "PARTICIONES");
		int tiempoEntreCompactaciones = config_get_int_value(temporalArchivoConfig, "TIEMPOENTRECOMPACTACIONES");
		if(solicitadoPorMemoria)
		{
			int tamanio_buffer_metadatas = strlen(tabla) + strlen(consistencia) + sizeof(cantParticiones) + sizeof(tiempoEntreCompactaciones) + 4;
			char* auxbuffer = malloc(tamanio_buffer_metadatas + 1);
			strcpy(auxbuffer, tabla);
			strcat(auxbuffer, ",");
			strcat(auxbuffer, consistencia);
			strcat(auxbuffer, ",");
			char* auxPart = string_itoa(cantParticiones);
			strcat(auxbuffer, auxPart);
			free(auxPart);
			strcat(auxbuffer, ",");
			char* auxtime = string_itoa(tiempoEntreCompactaciones);
			strcat(auxbuffer, auxtime);
			free(auxtime);
			strcat(auxbuffer, ";");
			free(auxdir);
			free(direccionDeTableMetadata);
			config_destroy(temporalArchivoConfig);
			free(consistencia);
			return auxbuffer;
		}
		else
		{
			printf("Las características del metadata de la tabla %s son: \n", tabla);
			printf(" Consistencia: %s. \n Cantidad de Particiones: %i. \n Tiempo entre compactaciones: %i. \n",
					consistencia, cantParticiones, tiempoEntreCompactaciones);
			free(direccionDeTableMetadata);
			config_destroy(temporalArchivoConfig);
			free(auxdir);
			free(consistencia);
			return NULL;
		}
	}
}

t_list* mostrarTodosLosMetadatas(bool solicitadoPorMemoria, char* auxbuffer)
{
	DIR* directorioDeTablas;
	struct dirent* tdp;
	t_list* listTables = list_create();
	char* auxdir = malloc(strlen(punto_montaje) + 8);
	strcpy(auxdir, punto_montaje);
	strcat(auxdir, "Tables/");
	if(NULL == (directorioDeTablas = opendir(auxdir)))
	{
		logError( "FileSystem: error al acceder al directorio de tablas, abortando");
		auxbuffer = malloc(6);
		strcpy(auxbuffer, "error");
		closedir(directorioDeTablas);
	}
	else
	{
		if(solicitadoPorMemoria)
		{
			logInfo("FileSystem: se procede a construir el paquete a enviar a Memoria.");
			while(NULL != (tdp = readdir(directorioDeTablas)))
			{
				if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, "..")){}
				else
				{
					char* superBuffer = mostrarMetadataEspecificada(tdp->d_name, solicitadoPorMemoria);
					char* buffer = malloc(strlen(superBuffer) + 1);
					strcpy(buffer, superBuffer);
					list_add(listTables, buffer);
					free(superBuffer);
				}
			}
		}
		else
		{
			logInfo("FileSystem: se procede a mostrar el contenido de las tablas del File System.");
			while(NULL != (tdp = readdir(directorioDeTablas)))
			{
				if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, ".."))	{}
				else
				{
					char* superBuffer = mostrarMetadataEspecificada(tdp->d_name, solicitadoPorMemoria);
					free(superBuffer);
				}
			}
		}
	}
	free(tdp);
	closedir(directorioDeTablas);
	free(auxdir);
	return listTables;
}

int contarTablasExistentes()
{
	DIR* auxdir;
	char* puntodemontaje = malloc(strlen(punto_montaje) + 9);
	strcpy(puntodemontaje, punto_montaje);
	strcat(puntodemontaje, "Tables/");
	struct dirent* dr;
	if(NULL == (auxdir = opendir(puntodemontaje)))
	{
		logError("FileSystem: No se pudo acceder al directorio de tablas.");
		printf("Error al querer contar las tablas existentes");
		free(puntodemontaje);
		closedir(auxdir);
		return (0);
	}
	else
	{
		int contadorDirectorios = 0;
		while((dr = readdir(auxdir)) != NULL)
		{
			if(!strcmp(dr->d_name, ".") || !strcmp(dr->d_name, ".."))
			{
				//no hace nada
			}
			else
				contadorDirectorios++;
		}
		logInfo("FileSystem: La cantidad de directorios existente es: %i", contadorDirectorios);
		free(puntodemontaje);
		free(dr);
		closedir(auxdir);
		return contadorDirectorios;
	}
}

t_keysetter* selectKeyFS(char* tabla, uint16_t keyRecibida)
{
	int esDeTalKey(t_keysetter* chequeada)
	{
		return chequeada->key == keyRecibida;
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
	t_TablaEnEjecucion* tablaChequeada = list_find(tablasEnEjecucion, (void*) estaTabla);
	logInfo("FileSystem: Se empieza a revisar el contenido de los bloques asignados a la %s para encontrar la clave %i."
			, tabla, keyRecibida);
	char* particionARevisar;
	t_list* clavesDentroDeLosBloques = list_create();
	t_list* clavesPostParseo = list_create();
	char* direccionTabla = malloc(strlen(punto_montaje) + strlen(tabla) + 9);
	strcpy(direccionTabla, punto_montaje);
	strcat(direccionTabla, "Tables/");
	strcat(direccionTabla, tabla);
	strcat(direccionTabla, "/");
	DIR* table = opendir(direccionTabla);
	struct dirent* tdp;
	pthread_mutex_lock(&tablaChequeada->compactacionActiva);
	while(NULL != (tdp = readdir(table)))
	{
		if(string_ends_with(tdp->d_name, ".cfg"))
		{
			char* direccionMetadataTabla = malloc(strlen(direccionTabla) + strlen(tdp->d_name) + 1);
			strcpy(direccionMetadataTabla, direccionTabla);
			strcat(direccionMetadataTabla, tdp->d_name);
			t_config* metadata = config_create(direccionMetadataTabla);
			int particiones = config_get_int_value(metadata, "PARTICIONES");
			int particionObjetivo = keyRecibida%particiones;
			particionARevisar = string_itoa(particionObjetivo);
			config_destroy(metadata);
			free(direccionMetadataTabla);
		}
		else if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, "..")){}
		else if(!string_ends_with(tdp->d_name, ".tmp") || !string_ends_with(tdp->d_name, ".tmpc"))
		{
			char* direccionTemp = malloc(strlen(direccionTabla) + strlen(tdp->d_name) + 2);
			strcpy(direccionTemp, direccionTabla);
			strcat(direccionTemp, tdp->d_name);
			int partSize = obtenerTamanioArchivoConfig(direccionTemp);
			if(partSize == 0)
			{
				free(direccionTemp);
			}
			else
			{
				int i = 0;
				char** bloques = obtenerBloques(direccionTemp);
				char* clavesLeidas = malloc(partSize + 1);
				while(bloques[i] != NULL)
				{
					if(i == 0)
						strcpy(clavesLeidas, leerBloque(bloques[i]));
					else
						strcat(clavesLeidas, leerBloque(bloques[i]));
					i++;
				}
				list_add(clavesDentroDeLosBloques, clavesLeidas);
				liberadorDeArrays(bloques);
				free(direccionTemp);
			}
		}
	}
	free(tdp);
	closedir(table);


	char* direccionParticion = malloc(strlen(direccionTabla) + strlen(particionARevisar) + 5);
	strcpy(direccionParticion, direccionTabla);
	strcat(direccionParticion, particionARevisar);
	strcat(direccionParticion, ".bin");
	int tamanioParticion = obtenerTamanioArchivoConfig(direccionParticion);
	if(tamanioParticion == 0){	}
	else
	{
		char** bloques = obtenerBloques(direccionParticion);
		char* clavesLeidas = malloc(tamanioParticion + 1);
		int a = 0;
		while(bloques[a] != NULL)
		{
			if(a == 0)
			{
				char* aux = leerBloque(bloques[a]);
				strcpy(clavesLeidas, aux);
				free(aux);
			}
			else
			{
				char* aux = leerBloque(bloques[a]);
				strcat(clavesLeidas, aux);
				free(aux);
			}
			a++;
		}
		char** keyHandlerBeta = string_split(clavesLeidas, "\n");
		int recount = 0;
		while(keyHandlerBeta[recount] != NULL)
		{
			char* auxSend = malloc(strlen(keyHandlerBeta[recount]) + 3);
			strcpy(auxSend, keyHandlerBeta[recount]);
			strcat(auxSend, "\n");
			list_add(clavesDentroDeLosBloques, auxSend);
			recount++;
		}
		liberadorDeArrays(keyHandlerBeta);
		free(clavesLeidas);
		liberadorDeArrays(bloques);
	}
	pthread_mutex_unlock(&tablaChequeada->compactacionActiva);
	free(particionARevisar);
	free(direccionParticion);

	// Fin de primera parte del select que setea todos los arrays y listas necesarios para reevisar y comparar las claves

	// Parte 2 parser de lista de claves sacada de cada tmp.
	logInfo("FileSystem: Se procede a parsear los contenidos de los bloques de los .tmp y de la particion pertenecientes a la %s."
			, tabla);
	clavesPostParseo = parsearKeys(clavesDentroDeLosBloques);

	//Paso 3 Correr select
	t_list* keysettersDeClave = list_create();
	keysettersDeClave = list_filter(clavesPostParseo, (void*)esDeTalKey);
	t_keysetter* claveMasActualizada = malloc(sizeof(t_keysetter) + 3);
	if(!list_is_empty(keysettersDeClave))
	{
		list_sort(keysettersDeClave, (void*)chequearTimeKey);
		claveMasActualizada = list_get(keysettersDeClave, 0);
		logInfo("FileSystem: Se ha obtenido el valor más reciente de la key %i.", keyRecibida);
	}
	else
	{
		claveMasActualizada = NULL;
		logInfo("FileSystem: La key %i no fue impactada todavía en el File System.", keyRecibida);
	}
	list_destroy(keysettersDeClave);
	list_destroy(clavesPostParseo);
	list_destroy_and_destroy_elements(clavesDentroDeLosBloques, &free);
	free(direccionTabla);
	return claveMasActualizada;
}

char* escribirBloquesDeFs(char* todasLasClavesAImpactar, int tamanioUsado, char* tabla)
{
	logInfo("File System: con lo que llega del Compactador, inicio la escritura de las claves a los bloques correspondientes");
	char* direccionTabla = malloc(strlen(tabla) + strlen(punto_montaje) + 9);
	strcpy(direccionTabla, punto_montaje);
	strcat(direccionTabla, "Tables/");
	strcat(direccionTabla, tabla);
	strcat(direccionTabla, "/");
	int bloquesAUsar = (tamanioUsado/tamanio_bloques) + 1;
	char* bloquesAsignados = malloc(bloquesAUsar*(sizeof(int) + 1) + 3);
	strcpy(bloquesAsignados, "[");
	bool firstBlock = true;
	int tamanioOcupado = 0;
	int bloquesCorridos = 0;
	while(tamanioOcupado != tamanioUsado)
	{
		char* bloque = obtenerBloqueLibre();
		if(firstBlock)
		{
			escribirBloque(&bloquesCorridos, &tamanioOcupado, tamanioUsado, bloque, todasLasClavesAImpactar);
			strcat(bloquesAsignados, bloque);
			firstBlock = false;
		}
		else
		{
			escribirBloque(&bloquesCorridos, &tamanioOcupado, tamanioUsado, bloque, todasLasClavesAImpactar);
			strcat(bloquesAsignados, ",");
			strcat(bloquesAsignados, bloque);
		}
		free(bloque);
	}
	strcat(bloquesAsignados, "]");
	free(direccionTabla);
	return bloquesAsignados;
}

char* obtenerBloqueLibre()
{
	int a = 0;
	char* bloqueAEnviar = malloc(5);
	bloqueAEnviar = "";
	while(!strcmp(bloqueAEnviar, ""))
	{
		//Hago que consulte al bitmap por el primer bloque libre//
		if(!bitarray_test_bit(bitarray, a))
		{
			char* uaxb = string_itoa(a);
			bloqueAEnviar = malloc(strlen(uaxb) + 1);
			pthread_mutex_lock(&modifierBitArray);
			bitarray_set_bit(bitarray, a);
			msync(bitarraycontent, bitarrayfd, MS_SYNC);
			pthread_mutex_unlock(&modifierBitArray);
			strcpy(bloqueAEnviar, uaxb);
			free(uaxb);
		}
		else
		{
			a++;
		}
	}
	return bloqueAEnviar;
}

void escribirBloque(int* usedBlocks, int* seizedSize, int usedSize, char* block, char* clavesAImpactar)
{
	int a;
	int mmapsize;
	int stillUnsaved = usedSize - *seizedSize;
	if(tamanio_bloques > stillUnsaved)
		mmapsize = usedSize - *seizedSize;
	else
		mmapsize = tamanio_bloques;
	char* blockDirection = malloc(strlen(direccionFileSystemBlocks) + strlen(block) + 5);
	strcpy(blockDirection, direccionFileSystemBlocks);
	strcat(blockDirection, block);
	strcat(blockDirection, ".bin");
	int fd2 = open(blockDirection, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    ftruncate(fd2, mmapsize);
    if (fd2 == -1)
    {
       	logError("FileSystem: error al abrir el bloque %s, abortando sistema", block);
       	signalExit = true;
    }
    else
    {
    	char* mmaplocator = mmap(NULL, mmapsize + 1, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
    	for(a = 0; a < tamanio_bloques; a++)
    	{
    		if(*seizedSize != usedSize)
    		{
    			int alreadyUsedBlocks = *usedBlocks;
    			mmaplocator[a] = clavesAImpactar[(tamanio_bloques*alreadyUsedBlocks) + a];
    			*seizedSize = *seizedSize + 1;
    		}
    		else
    		{
    			*usedBlocks = *usedBlocks + 1;
    			break;
    		}
    	}
    	*usedBlocks = *usedBlocks + 1;
    	msync(mmaplocator, fd2, MS_SYNC);
    	munmap(mmaplocator, mmapsize);
    	close(fd2);
    	free(blockDirection);
    }
}

void limpiadorDeBloques(char* direccion)
{
	logInfo("File System: procedo a limpiar los bloques otorgados a la tabla.");
	DIR* tabla;
	struct dirent* tdp;
	tabla = opendir(direccion);
	while(NULL != (tdp = readdir(tabla)))
	{
		if(string_ends_with(tdp->d_name, ".cfg")){}
		else
		{
			if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, "..")){}
			else
			{
				char* direccionPart = malloc(strlen(direccion) + strlen(tdp->d_name) + 2);
				strcpy(direccionPart, direccion);
				strcat(direccionPart, "/");
				strcat(direccionPart, tdp->d_name);
				limpiarBloque(direccionPart);
				free(direccionPart);
			}
		}
	}
	closedir(tabla);
	logInfo("File System: todos los bloques fueron limpiados satisfactoriamente");
}

void limpiarBloque(char* direccionPart)
{
	int i = 0;
	char** bloques = obtenerBloques(direccionPart);
	pthread_mutex_lock(&modifierBitArray);
	while(bloques[i] != NULL)
	{
		char* direccionBloqueALiberar = malloc(strlen(direccionFileSystemBlocks) + strlen(bloques[i]) + 5);
		strcpy(direccionBloqueALiberar, direccionFileSystemBlocks);
		strcat(direccionBloqueALiberar, bloques[i]);
		strcat(direccionBloqueALiberar, ".bin");
		FILE* fd = fopen(direccionBloqueALiberar, "w");
		fclose(fd);
		int indexBit = atoi(bloques[i]);
		bitarray_clean_bit(bitarray, indexBit);
		free(direccionBloqueALiberar);
		i++;
	}
	liberadorDeArrays(bloques);
	msync(bitarraycontent, bitarrayfd, MS_SYNC);
	pthread_mutex_unlock(&modifierBitArray);
}

char* leerBloque(char* bloque)
{
	char* direccionBloque = malloc(strlen(direccionFileSystemBlocks) + strlen(bloque) + 5);
	strcpy(direccionBloque, direccionFileSystemBlocks);
	strcat(direccionBloque, bloque);
	strcat(direccionBloque, ".bin");
	FILE* partpointer = fopen(direccionBloque, "r+");
	fseek(partpointer, 0, SEEK_END);
	unsigned long partlength = (unsigned long)ftell(partpointer);
	char* contenidoBloque = calloc(1, partlength + 1);
	if(!contenidoBloque)
	{
		fclose(partpointer);
		logError("FileSystem: error al usar calloc para abrir el contenido del bloque %s.bin", bloque);
		free(direccionBloque);
		return "error";
	}
	else
	{
		rewind(partpointer);
		fread(contenidoBloque, partlength, 1, partpointer);
		fclose(partpointer);
		free(direccionBloque);
		logInfo("FileSystem: Se ha leído el contenido del bloque %s.bin.", bloque);
		return contenidoBloque;
	}
}

char** obtenerBloques(char* direccion)
{
	t_config* archivo = config_create(direccion);
	char* bloquesAsignados = strdup(config_get_string_value(archivo, "BLOCKS"));
	char** bloques = string_get_string_as_array(bloquesAsignados);
	free(bloquesAsignados);
	config_destroy(archivo);
	return bloques;
}

void killProtocolFileSystem()
{
	bitarray_destroy(bitarray);
	munmap(bitarraycontent, strlen(bitarraycontent));
	close(bitarrayfd);
	free(globalBitmapPath);
	free(direccionFileSystemBlocks);
	logInfo("FileSystem: El bitmap fue destruido y las direcciones globales del FileSystem fueron destruidas.");
}

