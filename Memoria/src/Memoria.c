#include "Memoria.h"

int main() {

	time_t ahora = time(NULL);
	printf("%ld\n", ahora);

	levantar_config();
	initThread();

	//NO BORRAR DEBIDO A QUE LO USAMOS EN BREVE
	/*socket_kernel = levantar_servidor(info_memoria.puerto);
	t_prot_mensaje* handshake_kernel = prot_recibir_mensaje(socket_kernel);
	printf("el mensaje es %s\n", (char*)handshake_kernel->payload);

	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");*/

	while(1);
	return EXIT_SUCCESS;
}


//________________________________FUNCIONES DE INICIALIZACION DE HILOS Y DEMAS________________________________________________//

void setearValores(){
	return;
}

void initThread(){

	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);

	pthread_join(threadConsola ,NULL);
}

void inicializarMemoria(){
	//nos lo pasa el file system el tamanio del value, pero por ahora es 4 por el ejemplo
	uint16_t tamanio_value = 4;
	//el time_t es en segundos



	int tamanio_pag = sizeof(uint16_t) + tamanio_value + sizeof(time_t);
	int cant_marcos = info_memoria.tamanio_mem / tamanio_pag;

	t_pagina* memoria = malloc(info_memoria.tamanio_mem);
	bool* marco_libre = (bool*)calloc(cant_marcos, sizeof(bool));

	for(int i = 0; i<cant_marcos; i++){
		marco_libre[i] = 1;
	}
}
