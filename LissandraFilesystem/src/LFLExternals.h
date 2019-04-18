#ifndef LFLEXTERNALS_H_
#define LFLEXTERNALS_H_

#include <stdio.h>
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

/* ESTRUCTURAS */
typedef struct {
	time_t timestamp;
	uint key;
	char* clave;
}t_keysetter;

/* VARIABLES GLOBALES */
t_log * loggerLFL;

#endif /* LFLEXTERNALS_H_ */
