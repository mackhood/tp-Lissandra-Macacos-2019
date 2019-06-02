#include "Request.h"


char* selectReq (char* nombre_tabla, uint16_t key) {

	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);

	if(segmento_buscado /*!= NULL*/){
		//si el segmento existe buscamos en la memoria el key solicitado
		t_est_pag* est_pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

		if(est_pagina_buscada /*!= NULL*/){
			//se lo mando al Kernel si esta
			char* value_solicitado = malloc(tamanio_value);
			memcpy(value_solicitado, memoria_principal+tamanio_pag*(est_pagina_buscada->offset)+sizeof(double)+sizeof(uint16_t), tamanio_value);
			printf("el value solicitado es %s\n", value_solicitado);
			return value_solicitado;
			//prot_enviar_mensaje(socket_kernel, KEY_SOLICITADA_SELECT, strlen(value_solicitado), value_solicitado);
		}
		//no se encuentra la pagina en memoria
		else{
			free(est_pagina_buscada);

			//se lo pido al fs porque no esta
			int largo_nombre_tabla = strlen(nombre_tabla);
			size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla;
			void* buffer = malloc(tamanio_buffer);

			memcpy(buffer, &key, sizeof(uint16_t));
			memcpy(buffer+sizeof(uint16_t), &largo_nombre_tabla, sizeof(int));
			memcpy(buffer+sizeof(uint16_t)+sizeof(int), nombre_tabla, largo_nombre_tabla);

			prot_enviar_mensaje(socket_fs, SOLICITUD_TABLA, tamanio_buffer, buffer);
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

			prot_destruir_mensaje(mensaje_con_tabla);

			segmento_buscado = buscarEinsertarEnMem(segmento_buscado, key, tiempo_a_insertar, value);

			//prueba
			printf("el nombre del segmento es: %s\n", segmento_buscado->nombre_tabla);

			t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

			double time_buscado;
			memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
			uint16_t key_buscada;
			memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
			char* value_buscado = malloc(tamanio_value);
			memcpy(value_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

			printf("el time de la pagina es %lf\n", time_buscado);
			printf("la key de la pagina es %d\n", key_buscada);
			printf("el value de la pagina es %s\n", value_buscado);

			free(buffer);
			return value;
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

		//se lo pido al fs porque no esta
		int largo_nombre_tabla = strlen(nombre_tabla);
		size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla;
		void* buffer = malloc(tamanio_buffer);

		memcpy(buffer, &key, sizeof(uint16_t));
		memcpy(buffer+sizeof(uint16_t), &largo_nombre_tabla, sizeof(int));
		memcpy(buffer+sizeof(uint16_t)+sizeof(int), nombre_tabla, largo_nombre_tabla);

		prot_enviar_mensaje(socket_fs, SOLICITUD_TABLA, tamanio_buffer, buffer);
		t_prot_mensaje* mensaje_con_tabla = prot_recibir_mensaje(socket_fs);

		//el fs nos manda tiempo + tamanio value + value
		double tiempo_a_insertar;
		int tamanio_value_mandado;

		memcpy(&tiempo_a_insertar, mensaje_con_tabla->payload, sizeof(double));
		memcpy(&tamanio_value_mandado, mensaje_con_tabla->payload+sizeof(double), sizeof(int));

		//lo mando con el '\0' para que se sepa identificar que terminó dentro de la memoria
		char* value = malloc(tamanio_value_mandado+1);
		memcpy(value, mensaje_con_tabla->payload+sizeof(double)+sizeof(int), tamanio_value_mandado);
		value[tamanio_value_mandado]='\0';

		prot_destruir_mensaje(mensaje_con_tabla);

		segmento_nuevo = buscarEinsertarEnMem(segmento_nuevo, key, tiempo_a_insertar, value);

		/*	agrego al segmento despues de insertar la pagina en memoria debido a que dentro de la funcion esa uso al LRU y puedo estar usando
		*	el journaling el cual me elimina al segmento en cuestion
		*/
		list_add(lista_segmentos, segmento_nuevo);

		//prueba
		printf("el nombre del segmento es: %s\n", segmento_nuevo->nombre_tabla);

		t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_nuevo);

		double time_buscado;
		memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
		uint16_t key_buscada;
		memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
		char* value_buscado = malloc(tamanio_value);
		memcpy(value_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

		printf("el time de la pagina es %lf\n", time_buscado);
		printf("la key de la pagina es %d\n", key_buscada);
		printf("el value de la pagina es %s\n", value_buscado);

		free(buffer);
		return value;
	}
}

double insertReq (char* nombre_tabla, uint16_t key, char* value) {

	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);

	if(segmento_buscado){
		t_est_pag* est_pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

		if(est_pagina_buscada){
			double nuevo_time = getCurrentTime();
			memcpy(memoria_principal+tamanio_pag*(est_pagina_buscada->offset), &nuevo_time, sizeof(double));
			memcpy(memoria_principal+tamanio_pag*(est_pagina_buscada->offset)+sizeof(double)+sizeof(uint16_t), value, tamanio_value);
			est_pagina_buscada->flag = 1;

			//prueba
			char* value_actualizado = malloc(tamanio_value);
			memcpy(value_actualizado, memoria_principal+(est_pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);
			printf("el value actualizado es %s\n", value_actualizado);

			return nuevo_time;
		}
		else{
			free(est_pagina_buscada);

			double nuevo_time = getCurrentTime();

			segmento_buscado = buscarEinsertarEnMem(segmento_buscado, key, nuevo_time, value);
			//preguntar si tiene que tener el 1 o puede ser 0 (es la primera vez que aparece en memoria)

			//prueba y ademas marco el flag en 1
			t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);
			pagina_buscada->flag = 1;

			double time_buscado;
			memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
			uint16_t key_buscada;
			memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
			char* value_actualizado = malloc(tamanio_value);
			memcpy(value_actualizado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

			printf("el time de la pagina es %lf\n", time_buscado);
			printf("la key de la pagina es %d\n", key_buscada);
			printf("el value de la pagina es %s\n", value_actualizado);

			return nuevo_time;
		}
	}
	else{
		free(segmento_buscado);
		printf("llegue hasta aca\n");

		t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
		segmento_nuevo->nombre_tabla = strdup(nombre_tabla);
		//printf("la tabla de paginas no existia\n");
		//if(segmento_nuevo->tabla_paginas.paginas == NULL){
		//printf("la tabla de paginas no existia\n");
		segmento_nuevo->tabla_paginas.paginas = list_create();

		double nuevo_time = getCurrentTime();

		segmento_nuevo = buscarEinsertarEnMem(segmento_nuevo, key, nuevo_time, value);
		//preguntar si tiene que tener el 1 o puede ser 0 (es la primera vez que aparece en memoria)

		/*	agrego al segmento despues de insertar la pagina en memoria debido a que dentro de la funcion esa uso al LRU y puedo estar usando
		*	el journaling el cual me elimina al segmento en cuestion
		*/
		list_add(lista_segmentos, segmento_nuevo);

		//prueba y ademas marco el flag en 1
		t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_nuevo);
		pagina_buscada->flag = 1;

		double time_buscado;
		memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
		uint16_t key_buscada;
		memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
		char* value_asignado = malloc(tamanio_value);
		memcpy(value_asignado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

		printf("el time de la pagina es %lf\n", time_buscado);
		printf("la key de la pagina es %d\n", key_buscada);
		printf("el value de la pagina es %s\n", value_asignado);

		return nuevo_time;
	}

}

t_prot_mensaje* createReq (char* nombre_tabla, int largo_nombre_tabla, char* tipo_consistencia, int largo_tipo_consistencia, int numero_particiones, int compaction_time){

	size_t tamanio_buffer = sizeof(int) + largo_nombre_tabla + sizeof(int) + largo_tipo_consistencia + sizeof(int) + sizeof(int);

	void* buffer = malloc(tamanio_buffer);
	memcpy(buffer, &largo_nombre_tabla, sizeof(int));
	memcpy(buffer+sizeof(int), nombre_tabla, largo_nombre_tabla);
	memcpy(buffer+sizeof(int)+largo_nombre_tabla, &largo_tipo_consistencia, sizeof(int));
	memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(int), tipo_consistencia, largo_tipo_consistencia);
	memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia, &numero_particiones, sizeof(int));
	memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia+sizeof(int), &compaction_time, sizeof(int));

	prot_enviar_mensaje(socket_fs, CREATE_TABLA, tamanio_buffer, buffer);

	t_prot_mensaje* mensaje_fs = prot_recibir_mensaje(socket_fs);
	free(buffer);

	return mensaje_fs;
}

void describe (char** args) {





}

void dropReq (char* nombre_tabla) {
	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);
	int posicion_del_segmento_en_lista;

	if(segmento_buscado){
		/* itero para saber la posicion del segmento en la lista de segmentos para luego realizar el list_remove
		 * en base a la posicion del mismo
		 */

		posicion_del_segmento_en_lista = buscarPosSeg(nombre_tabla);

		/*	Segmento a remover deberia dar el mismo segmento que segmento buscado pero lo necesito declarar de nuevo
		 * 	por el list_remove.
		 */
		t_segmento* segmento_a_remover = list_remove(lista_segmentos, posicion_del_segmento_en_lista);

//--
		int cant_paginas_seg = list_size(segmento_a_remover->tabla_paginas.paginas);

		printf("Libero los marcos asignados al segmento %s\n", segmento_a_remover->nombre_tabla);
		for(int i=0; i<cant_paginas_seg; i++){
			//Marco como libre todos los marcos ocupados por las paginas del segmento a liberar
			t_est_pag* pagina_a_remover = list_get(segmento_a_remover->tabla_paginas.paginas, i);
			estados_memoria[pagina_a_remover->offset] = LIBRE;
			printf("libero el marco %d asignado a la pagina %d\n", pagina_a_remover->offset, i);
		}

//--
		//destruyo la TP(t_list*) del segmento a remover y sus paginas. Luego libero la memoria asignada al segmento en cuestion
		list_destroy_and_destroy_elements(segmento_a_remover->tabla_paginas.paginas, &free);
		free(segmento_a_remover->nombre_tabla);
		free(segmento_a_remover);
	}
	else{
		printf("Que haces flaco no te das cuenta que no esta el segmento en memoria\n");
	}
}

void journal () {

//Encontrar paginas con el flag de modificado

	//Si bien no necesito ninguna otra variable como en los casos anterior, al utilizar funciones de orden superior, la meto dentro del
	//journaling para una mejor abstraccion
	bool _esPaginaModificada(void* pagina){

		t_est_pag* pagina_a_buscar = (t_est_pag*)pagina;
		return pagina_a_buscar->flag==1;
	}

	int cant_segmentos = list_size(lista_segmentos);
	char* nombre_tabla;

	if (cant_segmentos ==0)
	{
		log_info(loggerMem, "No existen segmentos para ejecutar el Journaling");
	}
	else {
		//itero segmentos
		for (int i=0; i<cant_segmentos; i++)
		{
			//tomo uno por uno c/segmento
			t_segmento* seg_buscado = list_get(lista_segmentos, i);

			//guardo nombre y lista de paginas del segmento
			nombre_tabla = seg_buscado->nombre_tabla;
			int tamanio_tabla = strlen(nombre_tabla);
			t_list* paginas_del_segmento = seg_buscado->tabla_paginas.paginas;

			//filtro las páginas que fueron modificadas
			t_list* paginas_modif_seg = list_filter(paginas_del_segmento, _esPaginaModificada);

			int cant_pag_modif = list_size(paginas_modif_seg);

			if (cant_pag_modif == 0)
			{
				log_info(loggerMem, "No existen páginas modificadas para ejecutar el Journaling");
			}
			else
			{

			//recorro cada página del segmento
				for (int j=0; j<cant_pag_modif; j++)
					{

						t_est_pag* pag_a_enviar = list_get(paginas_modif_seg, j);
						double timestamp_enviar;
						uint16_t key_enviar;
						char* value_enviar = malloc(tamanio_value);

						//guardo info de la página
						memcpy(&timestamp_enviar, memoria_principal+(pag_a_enviar->offset*tamanio_pag), sizeof(double));
						memcpy(&key_enviar, memoria_principal+(pag_a_enviar->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
						memcpy(value_enviar, memoria_principal+(pag_a_enviar->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

						int tamanio_value_enviar = strlen(value_enviar);
						//creo un buffer para enviar con tamaño de tabla, tabla, key, tamaño de value, value y timestamp
						size_t tamanio_buffer = sizeof(int) + tamanio_tabla + sizeof(uint16_t) + sizeof(int) + tamanio_value_enviar  + sizeof(double);
						void*buffer = malloc(tamanio_buffer);

						memcpy(buffer, &timestamp_enviar, sizeof(double));
						memcpy(buffer+sizeof(double), &tamanio_tabla, sizeof(int));
						memcpy(buffer+sizeof(double)+sizeof(int), nombre_tabla, tamanio_tabla);
						memcpy(buffer+sizeof(double)+sizeof(int)+tamanio_tabla, &key_enviar, sizeof(uint16_t));
						memcpy(buffer+sizeof(double)+sizeof(int)+tamanio_tabla+sizeof(uint16_t), &tamanio_value_enviar, sizeof(int));
						memcpy(buffer+sizeof(double)+sizeof(int)+tamanio_tabla+sizeof(uint16_t)+sizeof(int), value_enviar, tamanio_value_enviar);

						printf("la pagina del segmento %s posee: \n el timestamp %lf\n la key %d\n y el value %s\n", nombre_tabla, timestamp_enviar, key_enviar, value_enviar);

						//enviar por c/u una petición de insert al FileSystem

						prot_enviar_mensaje(socket_fs, JOURNALING_INSERT, tamanio_buffer, buffer);

						//posibles respuestas: INSERT_SUCCESSFUL, INSERT_FAILED_ON_MEMTABLE, INSERT_FAILURE

						t_prot_mensaje* mensaje_fs = prot_recibir_mensaje(socket_fs);

						switch(mensaje_fs->head){

						case INSERT_SUCCESSFUL:{
								printf("Los datos de la tabla fueron actualizados\n");
							}break;

						case INSERT_FAILED_ON_MEMTABLE:{
								printf("El insert falló en la memtable\n");
							}break;

						case INSERT_FAILURE:{
								printf("Hubo un error al insertar la tabla\n");
							}break;
							default:{
								break;
							}

						}
						prot_destruir_mensaje(mensaje_fs);

						free(buffer);
						//free(pag_a_enviar);
					}

			}
			//list_destroy(paginas_modif_seg);
		}

	}

	int cant_seg_antes = list_size(lista_segmentos);
	printf("Segmentos que habían: %d \n", cant_seg_antes);

	//elimino segmentos y paginas actuales
	eliminar_segmentos();

	int cant_seg_ahora=list_size(lista_segmentos);
	printf("Segmentos que quedaron: %d \n", cant_seg_ahora);

	liberar_marcos();
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
