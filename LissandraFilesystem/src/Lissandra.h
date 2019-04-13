#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <loggers.h>
#include <dirent.h>
#include <string.h>
#include <netdb.h> // Para getaddrinfo
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <commons/collections/list.h>
#include <conexiones.h>

/* ESTRUCTURAS */

t_list * hilos;
t_list * compactadores;
t_list * memorias;
t_list * memtable;


typedef struct {
	int tempstamp;
	int key;
	char* clave;
}t_keysetter;

typedef struct {
	t_list * keysetters;
}t_memtable;


/*VARIABLES GLOBALES*/
int retardo;
char* server_ip;
int server_puerto;
int socket_memoria;
int socketFD;

/* FUNCIONES */

void mainLissandra();
void setearValores(t_config * archivoConfig);
void iniciarServidor();
void inserKeysetter(t_keysetter * clave, t_memtable * tabla);



#endif /* LISSANDRA_H_ */
