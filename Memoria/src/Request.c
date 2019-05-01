#include "Request.h"


void selectt (char** args) {
	char* nombre_tabla = string_duplicate(args[1]);
	uint16_t key = atoi(args[2]);

	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);

	if(segmento_buscado /*!= NULL*/){
		//si el segmento existe buscamos en la memoria el key solicitado
		t_est_pag* est_pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

		if(est_pagina_buscada /*!= NULL*/){
			//se lo mando al Kernel si esta
			char* value_solicitado = malloc(tamanio_value);
			memcpy(value_solicitado, memoria_principal+tamanio_pag*(est_pagina_buscada->offset)+sizeof(double)+sizeof(uint16_t), tamanio_value);
			printf("el value solicitado es %s\n", value_solicitado);
			prot_enviar_mensaje(socket_kernel, KEY_SOLICITADA_SELECT, strlen(value_solicitado), value_solicitado);
		}
		//no se encuentra la pagina en memoria
		else{
			free(est_pagina_buscada);

			//se lo pido al fs porque no esta
			prot_enviar_mensaje(socket_fs, SOLICITUD_TABLA, sizeof(uint16_t), &key);
			t_prot_mensaje* mensaje_con_tabla = prot_recibir_mensaje(socket_fs);

			//el fs nos manda tiempo + tamanio value + value
			double tiempo_a_insertar;
			int tamanio_value_mandado;

			memcpy(&tiempo_a_insertar, mensaje_con_tabla->payload, sizeof(double));
			memcpy(&tamanio_value_mandado, mensaje_con_tabla->payload+sizeof(double), sizeof(int));

			//lo mando con el '\0' para que se sepa identificar que termino dentro de la memoria
			char* value = malloc(tamanio_value_mandado+1);
			memcpy(value, mensaje_con_tabla->payload+sizeof(double)+sizeof(int), tamanio_value_mandado);
			value[tamanio_value_mandado]='\0';

			//prot_destruir_mensaje

			t_est_pag* nueva_est_pag = buscarEinsertarEnMem(segmento_buscado, key, tiempo_a_insertar, value);

			//prueba
			printf("el nombre del segmento es: %s\n", segmento_buscado->nombre_tabla);

			t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

			uint16_t key_buscada;
			memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
			char* value_buscado = malloc(tamanio_value);
			memcpy(value_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

			printf("la key de la pagina es %d\n", key_buscada);
			printf("el value de la pagina es %s\n", value_buscado);
		}
	}
	//no se encuentra el segmento
	else{
		free(segmento_buscado);
		printf("llegue hasta aca\n");
		//creo segmento
		t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
		segmento_nuevo->nombre_tabla = strdup(nombre_tabla);
		segmento_nuevo->tabla_paginas.paginas = list_create();
		list_add(lista_segmentos, segmento_nuevo);

		//pido fs la data
		//se lo pido al fs porque no esta
		prot_enviar_mensaje(socket_fs, SOLICITUD_TABLA, sizeof(uint16_t), &key);
		t_prot_mensaje* mensaje_con_tabla = prot_recibir_mensaje(socket_fs);

		//el fs nos manda tiempo + tamanio value + value
		double tiempo_a_insertar;
		int tamanio_value_mandado;

		memcpy(&tiempo_a_insertar, mensaje_con_tabla->payload, sizeof(double));
		memcpy(&tamanio_value_mandado, mensaje_con_tabla->payload+sizeof(double), sizeof(int));

		//lo mando con el '\0' para que se sepa identificar que termino dentro de la memoria
		char* value = malloc(tamanio_value_mandado+1);
		memcpy(value, mensaje_con_tabla->payload+sizeof(double)+sizeof(int), tamanio_value_mandado);
		value[tamanio_value_mandado]='\0';

		//prot_destruir_mensaje

		t_est_pag* nueva_est_pag = buscarEinsertarEnMem(segmento_nuevo, key, tiempo_a_insertar, value);

		//prueba
		printf("el nombre del segmento es: %s\n", segmento_nuevo->nombre_tabla);

		t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_nuevo);

		uint16_t key_buscada;
		memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
		char* value_buscado = malloc(tamanio_value);
		memcpy(value_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

		printf("la key de la pagina es %d\n", key_buscada);
		printf("el value de la pagina es %s\n", value_buscado);
	}
}

void insert (char** args) {
	char* nombre_tabla = string_duplicate(args[1]);
	uint16_t key = atoi(args[2]);
	char* value = string_duplicate(args[2]);

	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);



}
void create (char** args) {





}
void describe (char** args) {





}
void drop (char** args) {





}
void journal (char** args) {





}
void add (char** args) {





}
void run (char** args) {





}
void readLatency (char** args) {





}
void writeLatency (char** args) {





}
void reads (char** args) {





}
void writes (char** args) {





}
void memoryLoad (char** args) {





}


/*buscarsegmento
if(segmentoBuscado){
	buscarpagina
	if(paginaBuscada){
		envio value
	}
	else{
		creoPagina
		pido al fs data
		chequearLugaresEinsertar
	}
}
else{
	creoSegmento
	creoPagina
	pido al fs data
	chequearLugaresEinsertar
}

funcion para creacion de pagina*/
