#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "../../SharedLibrary/protocolo.h"
#include "../../SharedLibrary/conexiones.h"
#include <stdint.h>
#include "config_memoria.h"
#include "Consola.h"
#include <time.h>

typedef struct{
	size_t key;
	time_t timestamp;
	char* value;
} t_pagina;

typedef struct{
	int nro_pag;
	//es una lista de t_paginas
	t_list* pagina;
	int flag;
}t_tabla_pag;

typedef struct{
	char* nombre_tabla;
	t_tabla_pag* tabla_paginas;
}t_segmento;

t_list* lista_segmentos;

int socket_kernel;
int socket_fs;

void setearValores();
void initThread();


pthread_t threadConsola;


#endif /* MEMORIA_H_ */




