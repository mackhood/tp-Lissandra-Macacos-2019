#include "Memoria.h"

int main() {

	levantar_config();
	levantarLogs();
	levantarConexion();
	levantarEstrMemorias();
	initThread();

	while(1);
	return EXIT_SUCCESS;
}


//________________________________FUNCIONES DE INICIALIZACION DE HILOS Y DEMAS________________________________________________//

void levantarLogs(){

	char* memoria_log_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/Memoria/Memoria.log");
	loggerMem = crearLogger(memoria_log_ruta, "Memoria");

	log_info(loggerMem, "Se han levantado los logs");

	//Logeo si se levantan correctamente las configs
	char*memoria_config_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/Memoria/memoria.config");
	leerConfig(memoria_config_ruta, loggerMem);
}

void initThread(){

	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);

	int* kernel = (int*) malloc (sizeof(int));
	*kernel = socket_kernel;
	pthread_create(&threadReqKernel, NULL, (void*)escucharKernel, kernel);

	pthread_join(threadConsola ,NULL);
	pthread_join(threadReqKernel, NULL);
}

void levantarConexion(){

	//me conecto al fs primero y me manda el tamanio_value
	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");
	prot_enviar_mensaje(socket_fs, HANDSHAKE, 0, NULL);
	t_prot_mensaje* handshake = prot_recibir_mensaje(socket_fs);
	tamanio_value = *((int*)handshake->payload);
	prot_destruir_mensaje(handshake);
	log_info(loggerMem, "Se ha conectado la memoria con el File System");

	//levanto servidor para Kernel
	socket_escucha = levantar_servidor(info_memoria.puerto);

	struct sockaddr_in direccion_cliente;
	unsigned int tamanio_direccion = sizeof(direccion_cliente);

	socket_kernel = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion);
	printf("se ha conectado el kernel\n");

}

void levantarEstrMemorias(){

	//el tamanio_value nos lo pasa el fs por el handshake
	//la key es un uint16_t, tiene 16 bits por ende 2 bytes y no abarca numeros negativos (un int 4 bytes por ende 32 bits y si abarca negativos)
	//el time_t es en segundos (utilizado dentro de getCurrentTime)
	//el double ocupa 8 bytes

	//variables iniciales
	//tamanio_value = 4; //copio la del config del fs por ahora
	tamanio_pag = sizeof(uint16_t) + tamanio_value + sizeof(double);
	cant_paginas = info_memoria.tamanio_mem/tamanio_pag;

	//levanto memoria
	memoria_principal = malloc(info_memoria.tamanio_mem);

	//tengo un estados memoria para identificar frames libres, los levanto en LIBRE
	estados_memoria = malloc(sizeof(t_estado) * cant_paginas);

	for(int i=0; i<cant_paginas; i++){
		estados_memoria[i] = LIBRE;
	}

	//creo la lista de segmentos
	lista_segmentos = list_create();

	//creo el mutex
	pthread_mutex_init(&mutex_estructuras_memoria, NULL);

	//_____________________________PRUEBA________________________________//
	/*double timestamp = 1876;
	uint16_t key = 16;
	char* value = malloc(tamanio_value);
	memcpy(value, "abc", tamanio_value);

	memcpy(memoria_principal+(tamanio_pag*4), &timestamp, sizeof(double));
	memcpy(memoria_principal+(tamanio_pag*4)+sizeof(double), &key, sizeof(uint16_t));
	memcpy(memoria_principal+(tamanio_pag*4)+sizeof(double)+sizeof(uint16_t), value, tamanio_value);

	uint16_t key_recolectada;
	memcpy(&key_recolectada, memoria_principal+(tamanio_pag*4)+sizeof(double), sizeof(uint16_t));
	printf("en el marco 4 deberia estar la key %d\n", key_recolectada);*/

	//______________________________PRUEBA_______________________________//
}

void setearValores(){
	return;
}

