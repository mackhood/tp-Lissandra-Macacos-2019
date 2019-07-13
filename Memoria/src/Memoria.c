#include "Memoria.h"

int main() {

	//logueo todos los datos de configuracion
	initMutexlog(MEMORIA_LOG_PATH,PROGRAM_NAME,0,LOG_LEVEL_TRACE);
	levantar_config();
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

void levantarConexion(){

	logInfo("[Main]: Se procedera a conectar al fileSystem");
	//me conecto al fs primero y me manda el tamanio_value
	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");
	prot_enviar_mensaje(socket_fs, HANDSHAKE, 0, NULL);
	t_prot_mensaje* handshake = prot_recibir_mensaje(socket_fs);
	tamanio_value = *((int*)handshake->payload);
	prot_destruir_mensaje(handshake);
	logInfo("[Main]: Se ha conectado la memoria con el File System y nos ha mandado el TV %d", tamanio_value);

	logInfo("[Main]: Se procedera a levantar el servidor");
	//levanto servidor para Kernel y otras Memorias
	socket_escucha = levantar_servidor(info_memoria.puerto);

}

void levantarEstrMemorias(){

	//el tamanio_value nos lo pasa el fs por el handshake
	//la key es un uint16_t, tiene 16 bits por ende 2 bytes y no abarca numeros negativos (un int 4 bytes por ende 32 bits y si abarca negativos)
	//el time_t es en segundos (utilizado dentro de getCurrentTime)
	//el double ocupa 8 bytes

	logInfo("[Main]: Levantamos las estructuras principales");

	//variables iniciales
	tamanio_pag = sizeof(uint16_t) + tamanio_value + sizeof(double);
	cant_paginas = info_memoria.tamanio_mem/tamanio_pag;

	logInfo("[Main]: El tamanio de las paginas es %d", tamanio_pag);
	logInfo("[Main]: La cantidad de paginas es %d", cant_paginas);

	logInfo("[Main]: Se procede a levantar la memoria principal con un tamanio de %d", info_memoria.tamanio_mem);
	logInfo("[Main]: Se procede a levantar el bitarray para identificar los frames libres");


	//levanto memoria
	memoria_principal = malloc(info_memoria.tamanio_mem);

	//tengo un estados memoria para identificar frames libres, los levanto en LIBRE
	estados_memoria = malloc(sizeof(t_estado) * cant_paginas);

	for(int i=0; i<cant_paginas; i++){
		estados_memoria[i] = LIBRE;
	}

	logInfo("[Main]: Levantamos las estructuras restantes (lista segmentos, mutex, variable global para journal");

	//creo la lista de segmentos
	lista_segmentos = list_create();

	//creo el mutex
	pthread_mutex_init(&mutex_estructuras_memoria, NULL);

	//inicializo la variable global se_hizo_journal en false por las dudas (aunque por defecto deberia ser false)
	se_hizo_journal = false;
}

void initThread(){

	logInfo("[Main]: Levantamos hilos de consola, aceptarClientes y gossiping");

	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);
	pthread_create(&threadAceptacionAhre, NULL, (void*)aceptarClientes, NULL);
	pthread_create(&threadGossiping, NULL, (void*)gossiping, NULL);
	pthread_create(&threadNotify, NULL, (void*)notify, NULL);

	pthread_detach(threadAceptacionAhre);
	pthread_detach(threadConsola);
	pthread_join(threadGossiping, NULL);
	pthread_join(threadNotify, NULL);
}

void setearValores(){
	return;
}
