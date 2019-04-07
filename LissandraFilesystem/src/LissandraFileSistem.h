#ifndef LISSANDRAFILESISTEM_H_
#define LISSANDRAFILESISTEM_H_

#include "Lissandra.h"
#include "Compactador.h"
#include "FileSistem.h"
#include "Consola.h"
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


/* VARIABLES GLOBALES */
t_log* loggerLFL;
t_list* hilos;

/* SEMAFOROS */

/* FUNCIONES */
int main(void);
void iniciarConsola();
void iniciarLissandra();



#endif /* LISSANDRAFILESISTEM_H_ */
