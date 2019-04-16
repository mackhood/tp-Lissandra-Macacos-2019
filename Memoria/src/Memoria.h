#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "../../SharedLibrary/protocolo.h"
#include "../../SharedLibrary/conexiones.h"
#include <stdint.h>
#include "config_memoria.h"
#include "Consola.h"
#include <time.h>

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
int socket_kernel;
int socket_fs;

//funciones
void setearValores();
void initThread();
void levantarEstrMemorias();

//funciones dentro de hilos
void escucharKernel();


pthread_t threadConsola;
pthread_t threadReqKernel;


#endif /* MEMORIA_H_ */




