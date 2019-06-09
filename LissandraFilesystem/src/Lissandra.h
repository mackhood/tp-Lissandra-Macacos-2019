#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include "LFLExternals.h"
#include "FileSistem.h"
#include "Compactador.h"

/* ESTRUCTURAS */


/*VARIABLES GLOBALES*/

int tamanio_memtable;
int socket_escucha;
int socketFD;
char* server_ip;
int server_puerto;
bool killthreads;

/* FUNCIONES */

void mainLissandra();
void iniciarServidor();
void initNotifier();
void escucharMemoria();
void setearValoresLissandra(t_config * archivoConfig);
int insertKeysetter(char* tablaRecibida, uint16_t keyRecibida, char* valueRecibido, double timestampRecibido);
t_keysetter* selectKey(char* tabla, uint16_t receivedKey);
int llamadoACrearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion);
int llamarEliminarTabla(char* tablaPorEliminar);
int perteneceATabla(t_Memtablekeys* key);
int esDeTalKey(t_Memtablekeys* chequeada);
char* describirTablas(char* tablaSolicitada, bool solicitadoPorMemoria);
void notifier();
void killProtocolLissandra();

#endif /* LISSANDRA_H_ */
