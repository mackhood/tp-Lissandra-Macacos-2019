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

//	int* memoria = (int*) malloc (sizeof(int));
//	*memoria = socket_memoria;
//	pthread_create(&threadMensajesMemoria,NULL, (void*)escucharMemoria, memoria);

	pthread_detach(threadReqKernel);
	pthread_detach(threadConsola);
//	pthread_detach(threadMensajesMemoria, NULL);
}

void setearValores(){
	return;
}

void gossiping(){

	pthread_mutex_lock(&mutex_gossiping_memoria);
	list_clean(tabla_gossip);

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

		//Consulto si se pudo conectar a la memoria seed
		if (socket_memoria == -3)
		{
			//Aca entro si NO se pudo conectar. Esto significa que, o no fue levantada, o se desconectó
			//Verifico si es la primera memoria a la que intento conectarme

			if (list_is_empty(tabla_gossip))
			{
			//No fue levantada -> Indico en mi tabla Gossip que sólo yo existo.
			logInfo("La memoria a la que se intenta conectar no fue levantada");

			t_est_memoria* nuevaMemoria = malloc(sizeof(t_est_memoria));

			nuevaMemoria->ip_memoria = info_memoria.ip_memoria;
			nuevaMemoria->puerto_memoria = info_memoria.puerto;
			nuevaMemoria->numero_memoria = info_memoria.numero_memoria;

			list_add(tabla_gossip, nuevaMemoria);
			}else
			{
				/* Si entro acá, la tabla gossip NO está vacía.
				 * Verifico si contengo la memoria en la tabla gossip.
				 * Si está, significa que fue desconectada en algún momento, por ende tengo que borrarla de mi tabla
				 */

				bool conteniaEnTabla (t_est_memoria* memoria)
				{
					if (memoria->puerto_memoria == puerto_a_conectar)
					{
						return 1;
					}
					return 0;
				}

				list_remove_and_destroy_by_condition(tabla_gossip, (void*)conteniaEnTabla, &free);
				//Acá elimino la memoria de mi tabla gossip, falta saber cómo hacer que se enteren las demas que no se conectan directamente con ella.

			}

		}
		else
		{
			/*	Si fue levantada -> intercambian su tabla gossip agregando nodos faltantes.
				Una memoria conoce las que conoce la otra.
			*/

			logInfo("Se conectó con otra memoria");

			//Solicito tabla de gossip a la otra memoria. Ella me va a mandar elemento por elemento.

			prot_enviar_mensaje(socket_memoria, SOLICITUD_GOSSIP, 0, NULL);

			t_prot_mensaje* mensaje_con_memorias = prot_recibir_mensaje(socket_memoria);

			//Recibo todas las memorias de la tabla gossip juntas contenidas en un char*

			int tamanio_mensaje;

			memcpy(&tamanio_mensaje,mensaje_con_memorias->payload,sizeof(int));

			char* mensaje = malloc(tamanio_mensaje + 1);

			memcpy(mensaje, mensaje_con_memorias->payload + sizeof(int), tamanio_mensaje);

			mensaje[tamanio_mensaje] = '\0';

			//Separo la info de las memorias por ; en datosTAbla

			char ** datosTabla = string_split(mensaje, ";");

			t_list* tablaDeOtraMemoria;

			//Creo una lista de tablas para guardar memorias directamente
			int q=0;

			//recibo númeroTabla, ip, puerto;

			while(datosTabla[q]!=NULL){

				char* numero_memoria = strtok(datosTabla[q],",");
				char* ip_memoria = strtok(NULL, ",");
				char* puerto_memoria = strtok(NULL, ",");

				t_est_memoria* nuevaMemoria = malloc(sizeof(t_est_memoria*));

				int nuevo_numero = atoi(numero_memoria);
				int nuevo_puerto = atoi(puerto_memoria);

				nuevaMemoria->numero_memoria = nuevo_numero;
				strcpy(nuevaMemoria->ip_memoria, ip_memoria);
				nuevaMemoria->puerto_memoria = nuevo_puerto;



				list_add(tablaDeOtraMemoria, nuevaMemoria);

				q++;

			}

			//La otra memoria me manda su tabla gossip (memoria por memoria) (numero + ip + puerto)
//			int numero_mandado;
//			char* ip_mandado;
//			int puerto_mandado;
//
//			memcpy(&numero_mandado, mensaje_con_memoria->payload, sizeof(int));
//			memcpy(&ip_mandado, mensaje_con_memoria->payload+sizeof(int), IP_SIZE);
//			memcpy(&puerto_mandado, mensaje_con_memoria->payload+sizeof(int)+IP_SIZE, sizeof(int));
//
			prot_destruir_mensaje(mensaje_con_memorias);

			int tam = list_size(tablaDeOtraMemoria);

			for (int j=0; j<tam; j++)
			{

				t_est_memoria* memoria_tabla = list_get(tablaDeOtraMemoria, j);


				bool contieneMemoria (void* memoria)
					{
						t_est_memoria* memoria_a_comparar = (void*)memoria;

						if(memoria_a_comparar->numero_memoria == memoria_tabla->numero_memoria)
							{return 1;}

						else
							{return 0;}
					}

				//verifico si contengo la memoria. Si no está, la agrego a mi tabla gossip.
				bool laContiene = list_any_satisfy(tabla_gossip, contieneMemoria);

				if(!laContiene)
							{
								t_est_memoria* memoriaAgregar = malloc(sizeof(t_est_memoria));

								memoriaAgregar->numero_memoria = memoria_tabla->numero_memoria;
								strcpy(memoriaAgregar->ip_memoria, memoria_tabla->ip_memoria);
								memoriaAgregar->puerto_memoria = memoria_tabla->puerto_memoria;

								list_add(tabla_gossip, memoriaAgregar);

							}
			}

		}

		i++;
	}

	pthread_mutex_unlock(&mutex_gossiping_memoria);

}
