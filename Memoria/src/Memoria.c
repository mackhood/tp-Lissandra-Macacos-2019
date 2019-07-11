#include "Memoria.h"

int main() {

	levantar_config();
	levantarLogs();
	levantarConexion();
	levantarEstrMemorias();
	initThread();
	pthread_mutex_t stop;
	pthread_mutex_init(&stop, NULL);
	pthread_mutex_lock(&stop);
	pthread_mutex_lock(&stop);
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

void levantarConexion(){

	//me conecto al fs primero y me manda el tamanio_value
	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");
	prot_enviar_mensaje(socket_fs, HANDSHAKE, 0, NULL);
	t_prot_mensaje* handshake = prot_recibir_mensaje(socket_fs);
	tamanio_value = *((int*)handshake->payload);
	prot_destruir_mensaje(handshake);
	log_info(loggerMem, "Se ha conectado la memoria con el File System");

	//levanto servidor para Kernel y otras Memorias
	socket_escucha = levantar_servidor(info_memoria.puerto);

	//levanto servidor para otras memorias
	//Se aceptan clientes cuando los haya
	// accept es una funcion bloqueante, si no hay ningun cliente esperando ser atendido, se queda esperando a que venga uno.
/*	while(  (socket_memoria = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion)) > 0)
			{
				puts("Se ha conectado una memoria");
				logInfo( "[Memoria]: Se conecto con otra Memoria");
		}
*/

}

void levantarEstrMemorias(){

	//el tamanio_value nos lo pasa el fs por el handshake
	//la key es un uint16_t, tiene 16 bits por ende 2 bytes y no abarca numeros negativos (un int 4 bytes por ende 32 bits y si abarca negativos)
	//el time_t es en segundos (utilizado dentro de getCurrentTime)
	//el double ocupa 8 bytes

	//variables iniciales
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

	//inicializo la variable global se_hizo_journal en false por las dudas (aunque por defecto deberia ser false)
	se_hizo_journal = false;
}

void initThread(){

	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);
	pthread_create(&threadReqKernel, NULL, (void*)AceptarKernel, NULL);
	pthread_create(&threadMensajesMemoria,NULL, (void*)AceptarMemoria, NULL);
	pthread_create(&threadGossiping, NULL, (void*)gossiping, NULL);

	pthread_detach(threadReqKernel);
	pthread_detach(threadConsola);
	pthread_detach(threadMensajesMemoria);
	pthread_detach(threadGossiping);
}

void setearValores(){
	return;
}
