#include "Memoria.h"

int main() {

	levantar_config();
	levantarLogs();
	levantarConexion();
	levantarEstrMemorias();
	initThread();
	//gossiping();

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

//	int* memoria = (int*) malloc (sizeof(int));
//	*memoria = socket_memoria;
//	pthread_create(&threadMensajesMemoria,NULL, (void*)escucharMemoria, memoria);

	pthread_join(threadConsola ,NULL);
	pthread_join(threadReqKernel, NULL);
//	pthread_detach(threadMensajesMemoria, NULL);
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

	//levanto servidor para otras memorias
	//Se aceptan clientes cuando los haya
	// accept es una funcion bloqueante, si no hay ningun cliente esperando ser atendido, se queda esperando a que venga uno.
	while(  (socket_memoria = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion)) > 0)
			{
				puts("Se ha conectado una memoria");
				logInfo( "[Memoria]: Se conecto con otra Memoria");
		}


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

void gossiping(){

	char** ip_seeds = info_memoria.ip_seeds;
	char** puerto_seeds = info_memoria.puerto_seeds;
	int i=0;

	//Comienza gossip: Se descubren y conocen demás memorias


			while (NULL != ip_seeds[i])
			{
				//Intento conectarme a la seed.

				char* ip_a_conectar = ip_seeds[i];
				char* puerto = puerto_seeds[i];

				int puerto_a_conectar = atoi(puerto);

				socket_memoria = conectar_a_memoria_flexible(ip_a_conectar, puerto_a_conectar, "Memoria Gossiping");

				if (socket_memoria == -3)
				{
					// Si no fue levantada -> Indico en mi tabla Gossip que sólo yo existo.
					logInfo("Falló conexion con memoria");

					t_est_memoria* nuevaMemoria = malloc(sizeof(t_est_memoria));

					nuevaMemoria->ip_memoria = info_memoria.ip_memoria;
					nuevaMemoria->puerto_memoria = info_memoria.puerto;
					nuevaMemoria->numero_memoria = info_memoria.numero_memoria;

					list_add(tabla_gossip, nuevaMemoria);

					break;

				}
				else
				{
					/*	Si fue levantada -> intercambian su tabla gossip agregando nodos faltantes.
						Una memoria conoce las que conoce la otra.
					*/


					logInfo("Se conectó con otra memoria");

					//Solicito tabla de gossip a la otra memoria. Ella me va a mandar elemento por elemento.

					prot_enviar_mensaje(socket_memoria, SOLICITUD_GOSSIP, 0, NULL);

					t_prot_mensaje* mensaje_con_memoria = prot_recibir_mensaje(socket_memoria);


					//La otra memoria me manda su tabla gossip (memoria por memoria) (numero + ip + puerto)

					//Contemplar envío de todas las memorias de la tabla gossip juntas  por medio de un char* y parsear.

					int numero_mandado;
					char* ip_mandado;
					int puerto_mandado;

					memcpy(&numero_mandado, mensaje_con_memoria->payload, sizeof(int));
					memcpy(ip_mandado, mensaje_con_memoria->payload+sizeof(int), IP_SIZE);
					memcpy(&puerto_mandado, mensaje_con_memoria->payload+sizeof(int)+IP_SIZE, sizeof(int));

					prot_destruir_mensaje(mensaje_con_memoria);

					int tamanio = list_size(tabla_gossip);
					bool laContiene = false;

					for (int i=0; i<tamanio; i++)
						{
							t_est_memoria* nodoActual = list_get(tabla_gossip, i);

							if(nodoActual->numero_memoria == numero_mandado)
							{
								laContiene = true;
								break;
							}
						}

					if(!laContiene)
					{
						t_est_memoria* nuevaMemoria = malloc(sizeof(t_est_memoria));

						nuevaMemoria->numero_memoria = numero_mandado;
						nuevaMemoria->ip_memoria = ip_mandado;
						nuevaMemoria->puerto_memoria = puerto_mandado;

						list_add(tabla_gossip, nuevaMemoria);

					}

				}

				i++;
			}

}
