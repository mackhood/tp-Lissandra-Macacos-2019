#include "Memoria.h"

int main() {

	levantar_config();
	levantar_logs();
	levantarConexion();
	levantarEstrMemorias();
	initThread();

	while(1);
	return EXIT_SUCCESS;
}


//________________________________FUNCIONES DE INICIALIZACION DE HILOS Y DEMAS________________________________________________//

void setearValores(){
	return;
}

void levantar_logs(){

	char* memoria_log_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/Memoria/Memoria.log");
	loggerMem = crearLogger(memoria_log_ruta, "Memoria");

	log_info(loggerMem, "Prueba de log");

	//Logeo si se levantan correctamente las configs
	char*memoria_config_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/Memoria/memoria.config");
	leerConfig(memoria_config_ruta, loggerMem);
}
void initThread(){

	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);
	//pthread_create(&threadReqKernel, NULL, (void*)escucharKernel, NULL);

	pthread_join(threadConsola ,NULL);
}

void levantarConexion(){

	//me conecto al fs primero
	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");
	prot_enviar_mensaje(socket_fs, HANDSHAKE, 0, NULL);
	t_prot_mensaje* handshake = prot_recibir_mensaje(socket_fs);
	tamanio_value = *((int*)handshake->payload);
	prot_destruir_mensaje(handshake);

	//levanto servidor para Kernel
/*	socket_escucha = levantar_servidor(info_memoria.puerto);

	struct sockaddr_in direccion_cliente;
	unsigned int tamanio_direccion = sizeof(direccion_cliente);

	socket_kernel = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion);

	//el kernel nos enviara un mensaje inicial que diga quien es
	t_prot_mensaje* mensaje = prot_recibir_mensaje(socket_kernel);

	uint16_t key_recibida;
	time_t hora_actual;
	int tamanio_value;

	memcpy(&key_recibida, mensaje->payload, sizeof(uint16_t));
	memcpy(&hora_actual, mensaje->payload+sizeof(uint16_t), sizeof(time_t));
	memcpy(&tamanio_value, mensaje->payload+sizeof(uint16_t)+sizeof(time_t), sizeof(int));

	char* value = malloc(tamanio_value+1);
	memcpy(value, mensaje->payload+sizeof(uint16_t)+sizeof(time_t)+sizeof(int), tamanio_value);
	value[tamanio_value] = '\0';

	printf("el CLIENTE es %s y nos manda de prueba la key %d y la hora %ld\n\n", value, key_recibida, hora_actual);

	prot_destruir_mensaje(mensaje);
*/
}

void levantarEstrMemorias(){

	//nos lo pasa el file system el tamanio del value, pero por ahora es 4 por el ejemplo
	//la key es un uint16_t, tiene 16 bits por ende 2 bytes y no abarca numeros negativos (un int 4 bytes por ende 32 bits y si abarca negativos)
	//el time_t es en segundos (ya no lo usamos en la estructura pero esta bueno saberlo)
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

	lista_segmentos = list_create();

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








/*	//_____________________PRUEBA________________________________________//
	esto lo usabamos en el esquema anterior y ESTA MAL, pero era para probar que las otras funciones funcionen entonces podemos sacar
	algun que otro modo de prueba (creo)

	//creo la memoria el cual va a tener cant_paginas posiciones
	memoria_principal = list_create();

	for(int i=0; i<cant_paginas; i++){
		t_pagina* pagina = malloc(sizeof(t_pagina));
		pagina->value = malloc(tamanio_value);
		list_add(memoria_principal, (t_pagina*)pagina);
	}

	//creo lista de segmentos
	lista_segmentos = list_create();

	//creo segmento
	t_segmento* segmento_prueba = malloc(sizeof(t_segmento));
	segmento_prueba->nombre_tabla = "tabla1";
	segmento_prueba->tabla_paginas.paginas = list_create();

	//creo registro para la tabla
	t_est_pag* est_pagina = malloc(sizeof(t_est_pag));
	//est_pagina->nro_pag = 4;
	est_pagina->flag = 0;
	est_pagina->pagina = list_get(memoria_principal, 0);

	//creo la pagina a la cual apunta mi registro anterior
	est_pagina->pagina->key = 32;
	est_pagina->pagina->timestamp = getCurrentTime();
	est_pagina->pagina->value = "hol"; //incluye \0

	list_add(lista_segmentos, (t_segmento*)segmento_prueba);
	list_add((segmento_prueba->tabla_paginas.paginas), (t_est_pag*)est_pagina);

	t_segmento* segmento_definitivo = (t_segmento*)list_get(lista_segmentos, 0);
	t_list* tabla = segmento_definitivo->tabla_paginas.paginas;
	t_est_pag* est_pagina_def = (t_est_pag*)list_get(tabla, 0);

	printf("prueba para el levantamiento de memoria\n");
	printf("el flag es %d\n", est_pagina_def->flag);
	printf("la key de la pagina es %d\n", est_pagina_def->pagina->key);
	printf("el value de la pagina es %s\n", est_pagina_def->pagina->value);
	printf("el timestamp de la pagina es %lf\n", est_pagina_def->pagina->timestamp);
	printf("finalizo la prueba de levantar memoria\n\n");

	//busco key 32
	t_pagina* pagina_buscada = estaTablaYkeyEnMemoria("tabla1", 32);

	uint16_t key_buscado = pagina_buscada->key;
	printf("la key buscada es %d\n", key_buscado);


	//__________________________FIN DE PRUEBA___________________________//

*/


