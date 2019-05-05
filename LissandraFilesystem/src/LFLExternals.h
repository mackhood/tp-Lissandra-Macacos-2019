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

/* VARIABLES GLOBALES */
t_log * loggerLFL;
bool signalExit;
t_list * memtable;
char* punto_montaje;
t_list* tablasEnEjecucion;

#endif /* LFLEXTERNALS_H_ */
