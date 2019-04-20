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
#include <readline/readline.h>
#include <readline/history.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <commons/config.h>
//Las unicas biblios nuestras que podemos tener en el include para no generar dependencia circular
#include "../../SharedLibrary/protocolo.h"
#include "../../SharedLibrary/conexiones.h"

typedef struct{
	uint16_t key;
	time_t timestamp;
	char* value;
} t_pagina;

typedef struct{
	int nro_pag;
	t_pagina* pagina;
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

t_list* lista_segmentos;
size_t tamanio_pag;
size_t tamanio_value; //nos lo pasa el fs
size_t cant_lugares;

//sockets
int socket_escucha;
int socket_kernel;
int socket_fs;

//hilos
pthread_t threadConsola;
pthread_t threadReqKernel;

//funciones iniciales
void setearValores();
void initThread();
void levantarEstrMemorias();
void levantarConexion();

//funciones dentro de hilos
void escucharKernel();

//funciones para la administracion de memoria
t_pagina* estaTablaYkeyEnMemoria(char* tabla_a_buscar, uint16_t key);
t_segmento* buscarSegmento(char* tabla_a_buscar);
t_est_pag* buscarEstPagBuscada(uint16_t key, t_segmento* segmento_buscado);
void aplicar_LRU();
t_est_pag* crearPagina(time_t tiempo_de_pag, uint16_t key, int tamanio_value, char* value);
void chequearLugaresEinsertar(t_segmento* segmento, t_est_pag* est_pag_a_ins);



#endif /* ESTRUCTURASMEMORIA_H_ */
