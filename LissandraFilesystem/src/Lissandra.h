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
#include "LFLExternals.h"
#include "FileSistem.h"

/* ESTRUCTURAS */

t_list * hilos;
t_list * compactadores;
t_list * memorias;
t_list * memtable;




typedef struct {
	t_keysetter keysetter;
	char* tabla;
}t_memtablekey;


/*VARIABLES GLOBALES*/

int socket_memoria;
int socketFD;
int retardo;
char* server_ip;
int server_puerto;

/* FUNCIONES */

void mainLissandra();
void iniciarServidor();
void setearValoresLissandra(t_config * archivoConfig);
void insertKeysetter(char* tabla, uint16_t key, char* value, time_t timestamp);
t_keysetter selectKey(char* tabla, uint16_t receivedKey);
int llamadoACrearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion);

#endif /* LISSANDRA_H_ */
