#include "Memoria.h"

int main() {

	levantar_config();

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

void initThread(){

	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);
	//pthread_create(&threadReqKernel, NULL, (void*)escucharKernel, NULL);

	pthread_join(threadConsola ,NULL);
}

void levantarConexion(){

	//me conecto al fs primero
	//socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");

	//levanto servidor para Kernel
	socket_escucha = levantar_servidor(info_memoria.puerto);

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

}

void levantarEstrMemorias(){

	//nos lo pasa el file system el tamanio del value, pero por ahora es 4 por el ejemplo
	//la key es un uint16_t, tiene 16 bits por ende 2 bytes y no abarca numeros negativos (un int 4 bytes por ende 32 bits y si abarca negativos)
	//el time_t es en segundos
	tamanio_value = 4; //copio la del config del fs por ahora
	tamanio_pag = sizeof(uint16_t) + tamanio_value + sizeof(time_t);
	//la cantidad de lugares se ira restando a medida que tenga nuevas páginas
	cant_lugares = info_memoria.tamanio_mem / tamanio_pag;
	printf("probando tamanios: \nel sizeof int es de %d\nel sizeof uint es de %d\nel tamanio del time_t es %d\ntamanio pag %d\ncantidad de marcos %d\n\n",sizeof(int),sizeof(uint16_t),sizeof(time_t),tamanio_pag, cant_lugares);

	//_____________________PRUEBA________________________________________//

	//creo lista de segmentos
	lista_segmentos = list_create();

	//creo segmento
	t_segmento* segmento_prueba = malloc(sizeof(t_segmento));
	segmento_prueba->nombre_tabla = "tabla1";
	segmento_prueba->tabla_paginas.paginas = list_create();

	//creo registro para la tabla
	t_est_pag* est_pagina = malloc(sizeof(t_est_pag));
	est_pagina->nro_pag = 4;
	est_pagina->flag = 1;
	est_pagina->pagina = malloc(sizeof(t_pagina));

	//creo la pagina a la cual apunta mi registro anterior
	est_pagina->pagina->key = 32;
	est_pagina->pagina->timestamp = time(NULL);
	est_pagina->pagina->value = "hola";

	list_add(lista_segmentos, (t_segmento*)segmento_prueba);
	list_add((segmento_prueba->tabla_paginas.paginas), (t_est_pag*)est_pagina);

	t_segmento* segmento_definitivo = (t_segmento*)list_get(lista_segmentos, 0);
	t_list* tabla = segmento_definitivo->tabla_paginas.paginas;
	t_est_pag* est_pagina_def = (t_est_pag*)list_get(tabla, 0);

	printf("prueba para el levantamiento de memoria\n");
	printf("el nro de pagina es %d\n", est_pagina_def->nro_pag);
	printf("el flag es %d\n", est_pagina_def->flag);
	printf("la key de la pagina es %d\n", est_pagina_def->pagina->key);
	printf("el value de la pagina es %s\n", est_pagina_def->pagina->value);
	printf("el timestamp de la pagina es %ld\n", est_pagina_def->pagina->timestamp);
	printf("finalizo la prueba de levantar memoria\n\n");

	//busco key 32
	t_pagina* pagina_buscada = estaTablaYkeyEnMemoria("tabla1", 32);

	uint16_t key_buscado = pagina_buscada->key;
	printf("la key buscada es %d\n", key_buscado);

	//__________________________FIN DE PRUEBA_________________________________________________//
}


