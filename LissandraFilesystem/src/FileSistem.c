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
	bitarraycontent = malloc((int)(blocks/8) + 1);
    bitarrayfd = open(direccionBitmap, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
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
    }
}

void setearValoresFileSistem(t_config * archivoConfig)
{
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
		logError("FileSistem: El punto de montaje al que usted desea entrar no es accesible");
		return(1);
	}

	else
	{
		if(1 == existeTabla(nombre))
		{
			logInfo( "FileSystem: La tabla que usted quiere crear ya existe");
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

			logInfo( "FileSistem: Se procede a la creacion del directorio");
			mkdir(buff, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //creo el directorio de la tabla con sus respectivos permisos
			int results = crearMetadata(direccionFinal, consistencia, particiones, tiempoCompactacion); //Crea el archivio metadata
			if (results == 1)
			{
				perror("[ERROR] Error al crear Metadata, abortando");
				logError( "FileSistem: Error al crear Metadata, abortando");
				closedir(newdir);
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
					return(1);
				}
				else
				{
					closedir(newdir);
				}
			}
			free(direccionFinal);
			logInfo("FileSystem: Tabla creada satisfactoriamente");
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
		Linea3 = malloc(strlen(string_itoa(tiempoCompactacion)) + 28);
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
	newdir = opendir(punto_montaje);
	if(NULL == opendir)// reviso si el punto de montaje es accesible
	{
		perror("[ERROR] Punto de montaje no accesible");
		logError("FileSistem: El punto de montaje al que usted desea entrar no es accesible");
		closedir(newdir);
		return(1);
	}
	else
	{
		if(NULL == (checkdir = opendir(checkaux)))
		{
			logInfo( "FileSystem: La tabla que usted quiere borrar no existe");
			return(2);
		}
		else
		{
			closedir(checkdir);
			if (limpiadorDeArchivos(checkaux, tablaPorEliminar) == 1)
			{
				rmdir(checkaux);
				logInfo( "FileSystem: el directorio ha sido eliminado correctamente");
				return 0;
			}
			else
			{
				logError( "FileSystem: error al eliminar el directorio");
				return(3);
			}
		}
	}
	free(tablename);
	free(checkaux);
	free(puntodemontaje);
}

int limpiadorDeArchivos(char* direccion, char* tabla)
{
	bool estaTabla(t_TablaEnEjecucion* tablaDeLista)
	{
		char* tablaAux = malloc(strlen(tabla) + 1);
		strcpy(tablaAux, tabla);
		int cantCarac = strlen(tablaDeLista->tabla);
		char* tablaDeListaAux = string_new();
		//char* tablaDeListaAux = string_new();
		tablaDeListaAux = malloc(cantCarac + 1);
		strcpy(tablaDeListaAux, tablaDeLista->tabla);
		bool result = (0 == strcmp(tablaDeListaAux, tablaAux));
		return result;
	}
	limpiadorDeBloques(direccion);
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
		if(statusTemp == 0)
			continue;
		else
		{
			logError( "FileSystem: Error al eliminar temporales");
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
	checkdir = opendir(checkaux);
	if(NULL != checkdir)
		return 1;
	else
		return 0;
	closedir(checkdir);
	free(checkaux);
}

int mostrarMetadataEspecificada(char* tabla, int tamanio_buffer_metadatas, bool solicitadoPorMemoria, char* buffer)
{
	if(0 == existeTabla(tabla))
	{
		logInfo( "FileSystem: La tabla a la que quiere acceder no existe");
		return 0;
	}
	else
	{
		char* auxdir = string_new();
		auxdir = malloc(strlen(punto_montaje) + 8);
		strcpy(auxdir, punto_montaje);
		strcat(auxdir, "Tables/");
		char* direccionDeTableMetadata = string_new();
		direccionDeTableMetadata = malloc(strlen(auxdir) + strlen(tabla) + 15);
		strcpy(direccionDeTableMetadata, auxdir);
		strcat(direccionDeTableMetadata, tabla);
		strcat(direccionDeTableMetadata, "/Metadata.cfg");
		t_config * temporalArchivoConfig;
		temporalArchivoConfig = config_create(direccionDeTableMetadata);
		char* consistencia = strdup(config_get_string_value(temporalArchivoConfig, "CONSISTENCIA"));
		int	cantParticiones = config_get_int_value(temporalArchivoConfig, "PARTICIONES");
		int tiempoEntreCompactaciones = config_get_int_value(temporalArchivoConfig, "TIEMPOENTRECOMPACTACIONES");
		if(solicitadoPorMemoria)
		{
			tamanio_buffer_metadatas += strlen(consistencia) + sizeof(cantParticiones) + sizeof(tiempoEntreCompactaciones) + 3;
			buffer = realloc(buffer, tamanio_buffer_metadatas + 1);
			strcat(buffer, consistencia);
			strcat(buffer, ",");
			strcat(buffer, string_itoa(cantParticiones));
			strcat(buffer, ",");
			strcat(buffer, string_itoa(tiempoEntreCompactaciones));
			strcat(buffer, ";");
			free(auxdir);
			free(direccionDeTableMetadata);
			config_destroy(temporalArchivoConfig);
			return tamanio_buffer_metadatas;
		}
		else
		{
			printf("Las características del metadata de la tabla %s son: \n", tabla);
			printf("Consistencia: %s. \n Cantidad de Particiones: %i. \n Tiempo entre compactaciones: %i. \n",
					consistencia, cantParticiones, tiempoEntreCompactaciones);
			return 0;
		}
	}
}

void mostrarTodosLosMetadatas(bool solicitadoPorMemoria, char* buffer)
{
	DIR* directorioDeTablas;
	struct dirent* tdp;
	char* auxdir = string_new();
	auxdir = malloc(strlen(punto_montaje) + 8);
	strcpy(auxdir, punto_montaje);
	strcat(auxdir, "Tables/");
	if(NULL == (directorioDeTablas = opendir(auxdir)))
	{
		logError( "FileSystem: error al acceder al directorio de tablas, abortando");
		buffer = malloc(6);
		strcpy(buffer, "error");
		closedir(directorioDeTablas);
	}
	else
	{
		if(solicitadoPorMemoria)
		{
			int tamanio_buffer_metadatas = 0;
			logInfo( "FileSystem: se procede a construir el paquete a enviar a Memoria.");
			while(NULL != (tdp = readdir(directorioDeTablas)))
			{
				if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, "..")){}
				else
				{
					tamanio_buffer_metadatas += strlen(tdp->d_name) + 2;
					buffer = realloc(buffer, tamanio_buffer_metadatas);
					strcat(buffer, tdp->d_name);
					strcat(buffer, ",");
					int new_tamanio_buffer_metadatas = mostrarMetadataEspecificada(tdp->d_name, tamanio_buffer_metadatas, solicitadoPorMemoria, buffer);
					tamanio_buffer_metadatas = new_tamanio_buffer_metadatas;
				}
			}
		}
		else
		{
			int tamanio_buffer_metadatas = 0;
			logInfo( "FileSystem: se procede a mostrar el contenido de las tablas del File System.");
			while(NULL != (tdp = readdir(directorioDeTablas)))
			{
				if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, ".."))	{}
				else
				{
					tamanio_buffer_metadatas += strlen(tdp->d_name) + 2;
					buffer = realloc(buffer, tamanio_buffer_metadatas);
					int new_tamanio_buffer_metadatas = mostrarMetadataEspecificada(tdp->d_name, tamanio_buffer_metadatas, solicitadoPorMemoria, buffer);
					tamanio_buffer_metadatas = new_tamanio_buffer_metadatas;
				}
			}
		}
	}
}

int contarTablasExistentes()
{
	DIR* auxdir;
	char* puntodemontaje = string_new();
	puntodemontaje = malloc(strlen(punto_montaje) + 9);
	strcpy(puntodemontaje, punto_montaje);
	strcat(puntodemontaje, "Tables/");
	struct dirent* dr;
	if(NULL == (auxdir = opendir(puntodemontaje)))
	{
		logError( "FileSystem: No se pudo acceder al directorio de tablas.");
		printf("Error al querer contar las tablas existentes");
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
		logInfo( "FileSystem: La cantidad de directorios existente es: %i", contadorDirectorios);
		return contadorDirectorios;
	}
}

t_keysetter* selectKeyFS(char* tabla, uint16_t keyRecibida)
{
	int esDeTalKey(t_keysetter* chequeada)
	{
		return chequeada->key == keyRecibida;
	}

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
		}
		else if(!strcmp(tdp->d_name, ".") || !strcmp(tdp->d_name, "..")){}
		else if(string_ends_with(tdp->d_name, ".tmp"))
		{
			char* direccionTemp = malloc(strlen(direccionTabla) + strlen(tdp->d_name) + 2);
			strcpy(direccionTemp, direccionTabla);
			strcat(direccionTemp, "/");
			strcat(direccionTemp, tdp->d_name);
			FILE* temppointer = fopen(direccionTemp, "r+");
			fseek(temppointer, 0, SEEK_END);
			unsigned long templength = (unsigned long)ftell(temppointer);
			if(templength == 14)
			{
				fclose(temppointer);
			}
			else
			{
				fclose(temppointer);
				int i = 0;
				char** bloques = obtenerBloques(direccionTemp);
				int tamanio_temp = obtenerTamanioArchivoConfig(direccionTemp);
				char* clavesLeidas = malloc(tamanio_temp + 1);
				while(bloques[i] != NULL)
				{
					if(i == 0)
						strcpy(clavesLeidas, leerBloque(bloques[i]));
					else
						strcat(clavesLeidas, leerBloque(bloques[i]));
					i++;
				}
				list_add(clavesDentroDeLosBloques, clavesLeidas);
				free(bloques);
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
	char** bloques = obtenerBloques(direccionParticion);
	char* clavesLeidas = malloc(tamanioParticion + 1);
	int a = 0;
	while(bloques[a] != NULL)
	{
		if(a == 0)
			strcpy(clavesLeidas, leerBloque(bloques[a]));
		else
			strcat(clavesLeidas, leerBloque(bloques[a]));
		a++;
	}
	list_add(clavesDentroDeLosBloques, clavesLeidas);
	free(bloques);
	// Fin de primera parte del select que setea todos los arrays y listas necesarios para reevisar y comparar las claves

	// Parte 2 parser de lista de claves sacada de cada tmp.
	int parserListPointer = 0;
	char* keyHandler;
	while((keyHandler = list_get(clavesDentroDeLosBloques, parserListPointer)) != NULL)
	{
		t_keysetter* helpingHand = malloc(sizeof(t_keysetter) + 3);
		int parserPointer = 0;
		int handlerSize = strlen(keyHandler);
		char* key = malloc(sizeof(int) + 1);
		char* value = malloc(tamanio_value + 1);
		char* timestamp = malloc(sizeof(double) + 1);
		int status = 0;
		while(parserPointer < handlerSize)
		{
			switch(keyHandler[parserPointer])
			{
			case ',':
			{
				break;
			}
			case '\n':
			{
				break;
			}
			default:
			{
				switch(status)
				{
				case 0:
				{
					break;
				}
				case 1:
				{
					break;
				}
				case 2:
				{
					break;
				}
				}
				break;
			}
			}
		}
	}


	t_keysetter* claveMasActualizada;
	list_destroy(clavesPostParseo);
	list_destroy(clavesDentroDeLosBloques);
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
	}
	strcat(bloquesAsignados, "]");

	return bloquesAsignados;
}

char* obtenerBloqueLibre()
{
	int a = 0;
	char* bloqueAEnviar = string_new();
	while(!strcmp(bloqueAEnviar, ""))
	{
		//Hago que consulte al bitmap por el primer bloque libre//
		if(!bitarray_test_bit(bitarray, a))
		{
			bloqueAEnviar = malloc(strlen(string_itoa(a)) + 1);
			bitarray_set_bit(bitarray, a);
			strcpy(bloqueAEnviar, string_itoa(a));
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
    	char* mmaplocator = mmap(NULL, mmapsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0);
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
				FILE* partpointer = fopen(direccionPart, "r+");
				fseek(partpointer, 0, SEEK_END);
				unsigned long partlength = (unsigned long)ftell(partpointer);
				if(partlength == 14)
				{
					fclose(partpointer);
				}
				else
				{
					fclose(partpointer);
					int i = 0;
					char** bloques = obtenerBloques(direccionPart);
					while(bloques[i] != NULL)
					{
						char* direccionBloqueALiberar = malloc(strlen(direccionFileSystemBlocks) + strlen(bloques[i]) + 5);
						strcpy(direccionBloqueALiberar, direccionFileSystemBlocks);
						strcat(direccionBloqueALiberar, bloques[i]);
						strcat(direccionBloqueALiberar, ".bin");
						FILE* fd = fopen(direccionBloqueALiberar, "w");
						fclose(fd);
						free(direccionBloqueALiberar);
						i++;
					}
					free(bloques);
					free(direccionPart);
				}
			}
		}
	}
	closedir(tabla);
	logInfo("File System: todos los bloques fueron limpiados satisfactoriamente");
}

char* leerBloque(char* bloque)
{
	char* direccionBloque = malloc(strlen(direccionFileSystemBlocks) + strlen(bloque) + 5);
	FILE* partpointer = fopen(direccionBloque, "r+");
	fseek(partpointer, 0, SEEK_END);
	unsigned long partlength = (unsigned long)ftell(partpointer);
	char* contenidoBloque = calloc(1, partlength + 1);
	if(!contenidoBloque)
	{
		fclose(partpointer);
		logError("FileSystem: error al usar calloc para abrir el contenido del bloque %s.bin", bloque);
		return "error";
	}
	else
	{
		rewind(partpointer);
		fread(contenidoBloque, partlength, 1, partpointer);
		fclose(partpointer);
		free(direccionBloque);
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
	free(bitarraycontent);
	close(bitarrayfd);

}

