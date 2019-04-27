#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include "LFLExternals.h"
#include "FileSistem.h"

/* ESTRUCTURAS */

t_list * hilos;
t_list * compactadores;
t_list * memorias;
t_list * memtable;


/*VARIABLES GLOBALES*/

int tamanio_value;
int tamanio_memtable;
int socket_escucha;
int socketFD;
int retardo;
char* server_ip;
int server_puerto;
char* tablaAnalizada;
uint16_t keyAnalizada;

/* FUNCIONES */

void mainLissandra();
void iniciarServidor();
void escucharMemoria();
void setearValoresLissandra(t_config * archivoConfig);
void insertKeysetter(char* tablaRecibida, uint16_t keyRecibida, char* valueRecibido, double timestampRecibido);
t_keysetter* selectKey(char* tabla, uint16_t receivedKey);
int llamadoACrearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion);
int llamarEliminarTabla(char* tablaPorEliminar);
bool perteneceATabla(t_Memtablekeys* key);
bool chequearTimestamps(t_Memtablekeys* key1, t_Memtablekeys* key2);
int esDeTalKey(t_Memtablekeys* chequeada);
void describirTablas(char* tablaSolicitada);

#endif /* LISSANDRA_H_ */
