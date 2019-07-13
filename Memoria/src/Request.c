#include "Request.h"


char* selectReq (char* nombre_tabla, uint16_t key) {

	logInfo("[SELECT/REQ]: se procede a realizar el select");
	pthread_mutex_lock(&mutex_estructuras_memoria);

	logInfo("[SELECT/REQ]: se busca el segmento %s", nombre_tabla);
	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);

	if(segmento_buscado /*!= NULL*/){
		//si el segmento existe buscamos en la memoria el key solicitado
		logInfo("[SELECT/REQ]: el segmento existe en la lista");
		logInfo("[SELECT/REQ]: se busca la pagina %d", key);

		t_est_pag* est_pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

		if(est_pagina_buscada /*!= NULL*/){
			logInfo("[SELECT/REQ]: la pagina se encuentra en memoria");

			char* value_solicitado = malloc(tamanio_value);
			memcpy(value_solicitado, memoria_principal+tamanio_pag*(est_pagina_buscada->offset)+sizeof(double)+sizeof(uint16_t), tamanio_value);
			logInfo("[SELECT/REQ]: el value solicitado es %s", value_solicitado);

			pthread_mutex_unlock(&mutex_estructuras_memoria);
			return value_solicitado;
		}
		//no se encuentra la pagina en memoria
		else{
			logInfo("[SELECT/REQ]: la pagina no se encuentra en memoria");
			free(est_pagina_buscada);

			logInfo("[SELECT/REQ]: se envia la solicitud al fs");
			//se lo pido al fs porque no esta
			int largo_nombre_tabla = strlen(nombre_tabla);
			size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla;
			void* buffer = malloc(tamanio_buffer);

			memcpy(buffer, &key, sizeof(uint16_t));
			memcpy(buffer+sizeof(uint16_t), &largo_nombre_tabla, sizeof(int));
			memcpy(buffer+sizeof(uint16_t)+sizeof(int), nombre_tabla, largo_nombre_tabla);

			prot_enviar_mensaje(socket_fs, SOLICITUD_TABLA, tamanio_buffer, buffer);
			t_prot_mensaje* mensaje_con_tabla = prot_recibir_mensaje(socket_fs);
			usleep(info_memoria.retardo_fs*1000);

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

			logInfo("[SELECT/REQ]: el file system nos ha mandado el time %lf, el tamanio %d y el value %s", tiempo_a_insertar, tamanio_value_mandado, value);
			logInfo("[SELECT/REQ]: se procedera a crear e insertar la pagina en memoria");

			segmento_buscado = buscarEinsertarEnMem(segmento_buscado, key, tiempo_a_insertar, value);

			//verifico
			t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

			double time_buscado;
			memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
			uint16_t key_buscada;
			memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
			char* value_buscado = malloc(tamanio_value);
			memcpy(value_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

			logInfo("[SELECT/REQ]: verifico que el segmento sea %s, y la nueva pagina posee el time %lf, el key %d y el value %s",
					segmento_buscado->nombre_tabla,
					time_buscado,
					key_buscada,
					value_buscado);

			free(buffer);

			pthread_mutex_unlock(&mutex_estructuras_memoria);
			return value_buscado;
		}
	}
	//no se encuentra el segmento
	else{
		logInfo("[SELECT/REQ]: El segmento solicitado no existe");
		free(segmento_buscado);

		logInfo("[SELECT/REQ]: Se comenzara a crear el segmento");
		//creo segmento
		t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
		segmento_nuevo->nombre_tabla = strdup(nombre_tabla);
		segmento_nuevo->tabla_paginas.paginas = list_create();

		logInfo("[SELECT/REQ]: se envia la solicitud al fs");
		//se lo pido al fs porque no esta
		int largo_nombre_tabla = strlen(nombre_tabla);
		size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla;
		void* buffer = malloc(tamanio_buffer);

		memcpy(buffer, &key, sizeof(uint16_t));
		memcpy(buffer+sizeof(uint16_t), &largo_nombre_tabla, sizeof(int));
		memcpy(buffer+sizeof(uint16_t)+sizeof(int), nombre_tabla, largo_nombre_tabla);

		prot_enviar_mensaje(socket_fs, SOLICITUD_TABLA, tamanio_buffer, buffer);
		t_prot_mensaje* mensaje_con_tabla = prot_recibir_mensaje(socket_fs);
		usleep(info_memoria.retardo_fs*1000);

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

		logInfo("[SELECT/REQ]: el file system nos ha mandado el time %lf, el tamanio %d y el value %s", tiempo_a_insertar, tamanio_value_mandado, value);
		logInfo("[SELECT/REQ]: se procedera a crear e insertar la pagina en memoria");

		segmento_nuevo = buscarEinsertarEnMem(segmento_nuevo, key, tiempo_a_insertar, value);

		//Siempre se inserta el segmento nuevo a menos que se haya realizado el journal, en ese caso se agrega dentro de la funcion buscarEinsertar
		if(se_inserta_segmento){
			list_add(lista_segmentos, segmento_nuevo);

		}
		se_inserta_segmento = true;

		//verifico
		t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_nuevo);

		double time_buscado;
		memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
		uint16_t key_buscada;
		memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
		char* value_buscado = malloc(tamanio_value);
		memcpy(value_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

		logInfo("[SELECT/REQ]: verifico que el segmento sea %s, y la nueva pagina posee el time %lf, el key %d y el value %s",
				segmento_nuevo->nombre_tabla,
				time_buscado,
				key_buscada,
				value_buscado);

		free(buffer);

		pthread_mutex_unlock(&mutex_estructuras_memoria);
		return value_buscado;
	}
}

void insertReq (char* nombre_tabla, uint16_t key, char* value) {

	logInfo("[INSERT/REQ]: se procedera a realizar el insert");
	logInfo("[INSERT/REQ]: se procede a buscar el segmento solicitado");

	pthread_mutex_lock(&mutex_estructuras_memoria);
	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);

	if(segmento_buscado){
		logInfo("[INSERT/REQ]: el segmento existe en la lista de segmentos");
		logInfo("[INSERT/REQ]: se procedera a buscar la pagina solicitada");

		t_est_pag* est_pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);

		if(est_pagina_buscada){
			logInfo("[INSERT/REQ]: la pagina se encuentra en memoria por lo que cambio el flag de modificado a 1");

			double nuevo_time = getCurrentTime();
			memcpy(memoria_principal+tamanio_pag*(est_pagina_buscada->offset), &nuevo_time, sizeof(double));
			memcpy(memoria_principal+tamanio_pag*(est_pagina_buscada->offset)+sizeof(double)+sizeof(uint16_t), value, strlen(value) + 1);
			est_pagina_buscada->flag = 1;

			logInfo("[INSERT/REQ]: se inserta el nuevo time %lf en la pagina", nuevo_time);

			pthread_mutex_unlock(&mutex_estructuras_memoria);
		}
		else{
			logInfo("[INSERT/REQ]: la pagina no se encuentra en memoria");
			free(est_pagina_buscada);

			double nuevo_time = getCurrentTime();

			logInfo("[INSERT/REQ]: Se procedera a crear e insertar la pagina en memoria, pongo flag de modificado en 1");
			segmento_buscado = buscarEinsertarEnMem(segmento_buscado, key, nuevo_time, value);

			//verifico y ademas marco el flag en 1
			t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);
			pagina_buscada->flag = 1;

			double time_buscado;
			memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
			uint16_t key_buscada;
			memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
			char* value_actualizado = malloc(tamanio_value);
			memcpy(value_actualizado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

			logInfo("[INSERT/REQ]: verifico que el segmento sea %s, y la nueva pagina posee el time %lf, el key %d y el value %s",
					segmento_buscado->nombre_tabla,
					time_buscado,
					key_buscada,
					value_actualizado);

			pthread_mutex_unlock(&mutex_estructuras_memoria);
		}
	}
	else{
		logInfo("[INSERT/REQ]: el segmento no existe en la lista de segmentos");
		free(segmento_buscado);

		logInfo("[INSERT/REQ]: procedo a crear el segmento");

		t_segmento* segmento_nuevo = malloc(sizeof(t_segmento));
		segmento_nuevo->nombre_tabla = strdup(nombre_tabla);
		//printf("la tabla de paginas no existia\n");
		//if(segmento_nuevo->tabla_paginas.paginas == NULL){
		//printf("la tabla de paginas no existia\n");
		segmento_nuevo->tabla_paginas.paginas = list_create();

		double nuevo_time = getCurrentTime();

		logInfo("[INSERT/REQ]: se procedera a crear e insertar la pagina en memoria, marco flag de modificado en 1");
		segmento_nuevo = buscarEinsertarEnMem(segmento_nuevo, key, nuevo_time, value);

		//Siempre se insertara el segmento nuevo a menos que se haya realizado el journal, en ese caso el mismo se agregara dentro de la funcion buscarEinsertar
		if(se_inserta_segmento){
			list_add(lista_segmentos, segmento_nuevo);
		}
		se_inserta_segmento = true;

		//verifico y ademas marco el flag en 1
		t_est_pag* pagina_buscada = buscarEstPagBuscada(key, segmento_nuevo);
		pagina_buscada->flag = 1;

		double time_buscado;
		memcpy(&time_buscado, memoria_principal+(pagina_buscada->offset*tamanio_pag), sizeof(double));
		uint16_t key_buscada;
		memcpy(&key_buscada, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double), sizeof(uint16_t));
		char* value_asignado = malloc(tamanio_value);
		memcpy(value_asignado, memoria_principal+(pagina_buscada->offset*tamanio_pag)+sizeof(double)+sizeof(uint16_t), tamanio_value);

		logInfo("[INSERT/REQ]: verifico que el segmento sea %s, y la nueva pagina posee el time %lf, el key %d y el value %s",
				segmento_nuevo->nombre_tabla,
				time_buscado,
				key_buscada,
				value_asignado);

		pthread_mutex_unlock(&mutex_estructuras_memoria);
	}

}

t_prot_mensaje* createReq (char* nombre_tabla, int largo_nombre_tabla, char* tipo_consistencia, int largo_tipo_consistencia, int numero_particiones, int compaction_time){

	logInfo("[CREATE/REQ]: Se envia solicitud de CREATE al Filesystem");

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

t_prot_mensaje* dropReq (char* nombre_tabla) {

	logInfo("[DROP/REQ]: Comenzara a realizarse el DROP de la tabla %s", nombre_tabla);

	pthread_mutex_lock(&mutex_estructuras_memoria);
	t_segmento* segmento_buscado = buscarSegmento(nombre_tabla);

	if(segmento_buscado){

		logInfo("[DROP/REQ]: La tabla/segmento se encuentra en memoria");

		/* itero para saber la posicion del segmento en la lista de segmentos para luego realizar el list_remove
		 * en base a la posicion del mismo
		 */

		int posicion_del_segmento_en_lista = buscarPosSeg(nombre_tabla);
		logInfo("[DROP/REQ]: La posicion del segmento en la lista es %d", posicion_del_segmento_en_lista);

		/*	Segmento a remover deberia dar el mismo segmento que segmento buscado pero lo necesito declarar de nuevo
		 * 	por el list_remove.
		 */
		t_segmento* segmento_a_remover = list_remove(lista_segmentos, posicion_del_segmento_en_lista);

		int cant_paginas_seg = list_size(segmento_a_remover->tabla_paginas.paginas);

		logInfo("[DROP/REQ]: Comienzo a liberar los marcos asignados al segmento");
		for(int i=0; i<cant_paginas_seg; i++){
			//Marco como libre todos los marcos ocupados por las paginas del segmento a liberar
			t_est_pag* pagina_a_remover = list_get(segmento_a_remover->tabla_paginas.paginas, i);
			estados_memoria[pagina_a_remover->offset] = LIBRE;
			logInfo("[DROP/REQ]: Libero el marco %d asignado a la pagina %d", pagina_a_remover->offset, i);
		}

		logInfo("[DROP/REQ]: destruyo la TP del segmento removido y sus paginas. Luego libero la memoria asignada al segmento en cuestion");
		list_destroy_and_destroy_elements(segmento_a_remover->tabla_paginas.paginas, &free);
		free(segmento_a_remover->nombre_tabla);
		free(segmento_a_remover);
	}
	else{
		logError("[DROP/REQ]: El segmento solicitado no se encuentra en memoria");
	}

	logInfo("[DROP/REQ]: Le envío la solicitud al filesystem para que borre la tabla");

	int largo_nombre_tabla = strlen(nombre_tabla);
	int tamanio_buffer = sizeof(int) + largo_nombre_tabla;
	void* buffer = malloc(tamanio_buffer);
	memcpy(buffer, &largo_nombre_tabla, sizeof(int));
	memcpy(buffer + sizeof(int), nombre_tabla, largo_nombre_tabla);

	//mando solicitud de drop de tabla al FS
	prot_enviar_mensaje(socket_fs, TABLE_DROP, tamanio_buffer, buffer);
	t_prot_mensaje* respuesta = prot_recibir_mensaje(socket_fs);

	pthread_mutex_unlock(&mutex_estructuras_memoria);
	return respuesta;
}

void journalReq () {

	logInfo("[JOURNAL/REQ]: Comienza a realizarse el journal");

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
		logInfo("[JOURNAL/REQ]: No existen segmentos para ejecutar el Journaling");
	}
	else {

		logInfo("[JOURNAL/REQ]: Se realizara el chequeo de los %d segmentos existentes", cant_segmentos);
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
				logInfo("[JOURNAL/REQ]: No existen páginas modificadas del segmento evaluado");
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

						logInfo("[JOURNAL/REQ]: Envio pagina del segmento %s con timestamp %lf, la key %d, y el value %s", nombre_tabla, timestamp_enviar, key_enviar, value_enviar);

						//enviar por c/u una petición de insert al FileSystem

						prot_enviar_mensaje(socket_fs, JOURNALING_INSERT, tamanio_buffer, buffer);

						//posibles respuestas: INSERT_SUCCESSFUL, INSERT_FAILED_ON_MEMTABLE, INSERT_FAILURE

						t_prot_mensaje* mensaje_fs = prot_recibir_mensaje(socket_fs);

						logInfo("[JOURNAL/REQ]: recibimos la respuesta del filesystem");
						switch(mensaje_fs->head){

						case INSERT_SUCCESSFUL:{
								logInfo("[JOURNAL/REQ]: Se han actualizado los datos de la tabla en el filesystem");

							}break;

						case INSERT_FAILED_ON_MEMTABLE:{
								logError("[JOURNAL/REQ]: hubo un error en la memtable del filesystem");
							}break;

						case INSERT_FAILURE:{
								logInfo("[JOURNAL/REQ]: Hubo un error al insertar los datos en el fs");
							}break;
							default:{
								break;
							}

						}
						prot_destruir_mensaje(mensaje_fs);

						free(buffer);

						usleep(info_memoria.retardo_fs);
					}

			}
			//list_destroy(paginas_modif_seg);
		}

	}

	int cant_seg_antes = list_size(lista_segmentos);
	logInfo("[JOURNAL/REQ]: segmentos que habia %d", cant_seg_antes);

	//elimino segmentos y paginas actuales
	eliminar_segmentos();

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
