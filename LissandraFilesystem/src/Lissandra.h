#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <loggers.h>
#include <string.h>
#include <netdb.h> // Para getaddrinfo
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <commons/collections/list.h>
#include <conexiones.h>


/*VARIABLES GLOBALES*/
int retardo;
char* server_ip;
int server_puerto;
int socket_memoria;
int socketFD;

/* FUNIONES */

void setearValores(t_config * archivoConfig);





#endif /* LISSANDRA_H_ */
