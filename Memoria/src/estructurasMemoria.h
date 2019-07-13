#ifndef ESTRUCTURASMEMORIA_H_
#define ESTRUCTURASMEMORIA_H_

/*SE UTILIZA PARA PODER TENER TODOS LOS INCLUDE PARA LOS TIPOS DE DATOS QUE VAN A MANEJAR MIS MODULOS EN GENERAL,
 * NO IMPORTA SI DENTRO DE CONEXIONES Y PROTOCOLO INCLUIMOS LOS MISMOS INCLUDES PARA BIBLIOTECAS DEL SISTEMA, MIENTRAS
 * NO UTILICEMOS ESTE .h PARA INCLUIR .h DE MIS MODULOS ESTA TODO BIEN
 */

#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include "../../SharedLibrary/loggers.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <pthread.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
#include <sys/inotify.h>

//Las unicas biblios nuestras que podemos tener en el include para no generar dependencia circular
#include "../../SharedLibrary/protocolo.h"
#include "../../SharedLibrary/conexiones.h"
#include "../../SharedLibrary/auxiliaryFunctions.h"
#include "config_memoria.h"

#define MEMORIA_LOG_PATH "memoria.log"
#define PROGRAM_NAME "MEMORIA"

typedef struct{
	int nro_pag;
	uint16_t offset;
	int flag;
}t_est_pag;

typedef struct{
	//es una lista de t_est_pag
	t_list* paginas;
}t_tabla_pag;

typedef struct{
	char* nombre_tabla;
	t_tabla_pag tabla_paginas;
}t_segmento;

typedef enum{
	LIBRE,
	OCUPADO
}t_estado;

typedef struct{
	int numero_memoria;
	char* ip_memoria;
	int puerto_memoria;
}t_est_memoria;

t_list* lista_segmentos;
t_list* tabla_gossip;
size_t tamanio_pag;
size_t tamanio_value; //nos lo pasa el fs
size_t cant_paginas;
void* memoria_principal;
t_estado* estados_memoria;
bool se_hizo_journal;
bool se_inserta_segmento;

//sockets
int socket_escucha;
int socket_fs;
int socket_cliente;

//hilos
pthread_t threadConsola;
pthread_t threadAceptacionAhre;
pthread_t threadGossiping;
pthread_t threadNotify;

//logs
t_log* loggerMem;

//mutex
pthread_mutex_t mutex_estructuras_memoria;
pthread_mutex_t mutex_tabla_gossiping;

//funciones iniciales
void setearValores();
void initThread();
void levantarEstrMemorias();
void levantarConexion();
void gossiping();

//funciones para la administracion de memoria
t_segmento* buscarSegmento(char* tabla_a_buscar);
t_est_pag* buscarEstPagBuscada(uint16_t key, t_segmento* segmento_buscado);
int aplicarLRU();
t_segmento* buscarEinsertarEnMem(t_segmento* segmento, uint16_t key, double time_a_insertar, char* value);
int buscarPaginaLibre(); //ahora devuelvo un marco
void journalReq (); //Si bien es una request la pongo aca dentro debido a que el LRU debe utilizarla
void eliminar_segmentos();
void liberar_marcos();
int buscarPosSeg(char* nombre_segmento);

#endif /* ESTRUCTURASMEMORIA_H_ */
