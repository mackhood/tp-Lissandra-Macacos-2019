#ifndef LFLEXTERNALS_H_
#define LFLEXTERNALS_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <loggers.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <commons/collections/list.h>
#include <conexiones.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <commons/string.h>
#include <time.h>
#include <ftw.h>
#include <fts.h>
#include <auxiliaryFunctions.h>
#include <commons/bitarray.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>

/* ESTRUCTURAS */
typedef struct {
	double timestamp;
	uint16_t key;
	char* clave;
}__attribute__((packed)) t_keysetter;

typedef struct {
	t_keysetter* data;
	char* tabla;
}__attribute__((packed)) t_Memtablekeys;

typedef struct {
	char* tabla;
	int cantTemps;
}__attribute__((packed)) t_TablaEnEjecucion;

/* VARIABLES GLOBALES */
bool fileSystemFull;
int tamanio_value;
char* direccionFileSystemBlocks;
char* globalBitmapPath;
char* bitarraycontent;
int blocks;
int bitarrayfd;
t_bitarray* bitarray;
t_log * logger;
bool signalExit;
t_list * memtable;
char* punto_montaje;
t_list* tablasEnEjecucion;
pthread_mutex_t deathProtocol;
pthread_mutex_t compactacionActiva;
pthread_mutex_t dumpEnCurso;
pthread_mutex_t renombreEnCurso;

/* FUNCIONES GLOBALES */
int cantidadDeBloquesLibres();
int cantidadDeBloquesAOcupar();
char* timeForLogs();
int chequearTimestamps(t_Memtablekeys* key1, t_Memtablekeys* key2);
int chequearTimeKey(t_keysetter* key1, t_keysetter* key2);
int obtenerTamanioArchivoConfig(char* direccionArchivo);
t_keysetter* construirKeysetter(char* timestamp, char* key, char* value);
unsigned long obtenerTamanioArchivo(char* direccionArchivo);
t_list* parsearKeys(t_list* clavesAParsear);
t_list* inversaParsearKeys(t_list* clavesADesparsear);


#endif /* LFLEXTERNALS_H_ */
