#include "conexion_kernel.h"

void escucharYatenderKernel(int* kernel){

	//Comienzo a recibir peticiones
	int socket_k = *kernel;
	free(kernel);
	t_prot_mensaje* req_recibida = prot_recibir_mensaje(socket_k);

	switch(req_recibida->head){
		case SELECT_REQ:
		{
			int tamanio_nombre_tabla;
			char* nombre_tabla;
			uint16_t key;

			memcpy(&tamanio_nombre_tabla, req_recibida->payload, sizeof(int));
			nombre_tabla = malloc(tamanio_nombre_tabla+1);
			memcpy(nombre_tabla, req_recibida->payload+sizeof(int), tamanio_nombre_tabla);
			nombre_tabla[tamanio_nombre_tabla] = '\0';
			memcpy(&key, req_recibida->payload+sizeof(int)+ tamanio_nombre_tabla, sizeof(uint16_t));

			char* value_solicitado = selectReq(nombre_tabla, key);

			//Si es null significa que el filesystem no tiene la key para dicha tabla (esta explicado dentro de la request)
			if(value_solicitado){
				int largo_value = strlen(value_solicitado);
				int tamanio_buffer = sizeof(int) + largo_value;
				void* buffer = malloc(tamanio_buffer);

				memcpy(buffer,&largo_value,sizeof(int));
				memcpy(buffer+sizeof(int),value_solicitado,largo_value);

				prot_enviar_mensaje(socket_k, KEY_SOLICITADA_SELECT, tamanio_buffer, buffer);
				free(value_solicitado);
				free(buffer);
			}
			else{
				prot_enviar_mensaje(socket_k, SELECT_FAILURE, 0, NULL);
			}

			usleep(info_memoria.retardo_mp*1000);

			free(nombre_tabla);

		} break;

		case INSERT_REQ:
		{
			int tamanio_nombre_tabla;
			char* nombre_tabla;
			uint16_t key;
			int largo_value;
			char* value;

			memcpy(&tamanio_nombre_tabla, req_recibida->payload, sizeof(int));
			nombre_tabla = malloc(tamanio_nombre_tabla+1);
			memcpy(nombre_tabla, req_recibida->payload + sizeof(int), tamanio_nombre_tabla);
			nombre_tabla[tamanio_nombre_tabla] = '\0';
			memcpy(&key, req_recibida->payload + sizeof(int) + tamanio_nombre_tabla, sizeof(uint16_t));
			memcpy(&largo_value, req_recibida->payload +sizeof(int)+tamanio_nombre_tabla+sizeof(uint16_t), sizeof(int));
			value = malloc(largo_value+1);
			memcpy(value, req_recibida->payload+sizeof(int)+tamanio_nombre_tabla+sizeof(uint16_t)+sizeof(int), largo_value);
			value[largo_value] = '\0';

			bool se_hizo_insert = insertReq(nombre_tabla, key, value);
			if(se_hizo_insert){
				prot_enviar_mensaje(socket_k, INSERT_REALIZADO, 0, NULL);
			}
			else{
				prot_enviar_mensaje(socket_k, INSERT_FAILURE, 0, NULL);
			}

			usleep(info_memoria.retardo_mp*1000);

			free(nombre_tabla);
			free(value);

		} break;

		case CREATE_REQ:
		{
			int largo_nombre_tabla;
			char* nombre_tabla;
			int largo_tipo_consistencia;
			char* tipo_consistencia;
			int numero_particiones;
			int compaction_time;

			memcpy(&largo_nombre_tabla, req_recibida->payload, sizeof(int));
			nombre_tabla = malloc(largo_nombre_tabla+1);
			memcpy(nombre_tabla, req_recibida->payload+sizeof(int), largo_nombre_tabla);
			nombre_tabla[largo_nombre_tabla]='\0';
			memcpy(&largo_tipo_consistencia, req_recibida->payload+sizeof(int)+largo_nombre_tabla, sizeof(int));
			tipo_consistencia = malloc(largo_tipo_consistencia+1);
			memcpy(tipo_consistencia, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int), largo_tipo_consistencia);
			tipo_consistencia[largo_tipo_consistencia]='\0';
			memcpy(&numero_particiones, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia, sizeof(int));
			memcpy(&compaction_time, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia+sizeof(int), sizeof(int));

			t_prot_mensaje* mensaje_fs = createReq(nombre_tabla, largo_nombre_tabla, tipo_consistencia, largo_tipo_consistencia, numero_particiones, compaction_time);

			switch(mensaje_fs->head){
						case TABLA_CREADA_OK:{
							prot_enviar_mensaje(socket_k, TABLA_CREADA_OK, 0, NULL);
						}break;
						case TABLA_CREADA_YA_EXISTENTE:{
							prot_enviar_mensaje(socket_k, TABLA_CREADA_YA_EXISTENTE, 0, NULL);
						}break;
						case TABLA_CREADA_FALLO:{
							prot_enviar_mensaje(socket_k, TABLA_CREADA_FALLO, 0, NULL);
						}break;
						default:{
							break;
						}
					}

			usleep(info_memoria.retardo_fs*1000);
			usleep(info_memoria.retardo_mp*1000);

			prot_destruir_mensaje(mensaje_fs);
			free(nombre_tabla);
			free(tipo_consistencia);

		} break;

		case DROP_REQ:
		{
			int largo_nombre_tabla;
			char* nombre_tabla;

			memcpy(&largo_nombre_tabla, req_recibida->payload, sizeof(int));
			nombre_tabla = malloc(largo_nombre_tabla + 1);
			memcpy(nombre_tabla, req_recibida->payload+sizeof(int), largo_nombre_tabla);
			nombre_tabla[largo_nombre_tabla] = '\0';

			t_prot_mensaje* respuesta_fs = dropReq(nombre_tabla);


			switch(respuesta_fs->head){
				case TABLE_DROP_OK:{
					prot_enviar_mensaje(socket_k, TABLE_DROP_OK, 0, NULL);
				}break;
				case TABLE_DROP_NO_EXISTE:{
					prot_enviar_mensaje(socket_k, TABLE_DROP_NO_EXISTE, 0, NULL);
				}break;
				case TABLE_DROP_FALLO:{
					prot_enviar_mensaje(socket_k, TABLE_DROP_FALLO, 0, NULL);;
				}break;
				default:{
					break;
				}
			}

			usleep(info_memoria.retardo_fs*1000);
			usleep(info_memoria.retardo_mp*1000);

			prot_destruir_mensaje(respuesta_fs);
			free(nombre_tabla);

		} break;

		case DESCRIBE_REQ:
		{
			if(req_recibida->tamanio_total>4){
				int largo_nombre_tabla;
				char* nombre_tabla;

				memcpy(&largo_nombre_tabla, req_recibida->payload, sizeof(int));
				nombre_tabla = malloc(largo_nombre_tabla+1);
				memcpy(nombre_tabla, req_recibida->payload+sizeof(int), largo_nombre_tabla);
				nombre_tabla[largo_nombre_tabla]='\0';

				int tamanio_buffer = sizeof(int) + largo_nombre_tabla;
				void* buffer = malloc(tamanio_buffer);

				memcpy(buffer, &largo_nombre_tabla, sizeof(int));
				memcpy(buffer+sizeof(int), nombre_tabla, largo_nombre_tabla);

				prot_enviar_mensaje(socket_fs, DESCRIBE, tamanio_buffer, buffer);
				t_prot_mensaje* data_del_fs = prot_recibir_mensaje(socket_fs);

				if(data_del_fs->head == POINT_DESCRIBE){
					int largo_descripcion;
					char* descripcion_tabla;

					memcpy(&largo_descripcion, data_del_fs->payload, sizeof(int));
					descripcion_tabla = malloc(largo_descripcion+1);
					memcpy(descripcion_tabla, data_del_fs->payload+sizeof(int), largo_descripcion);
					descripcion_tabla[largo_descripcion]='\0';

					int tamanio_buffer_kernel = sizeof(int)+largo_descripcion;
					void* buffer_kernel = malloc(tamanio_buffer_kernel);

					memcpy(buffer_kernel, &largo_descripcion, sizeof(int));
					memcpy(buffer_kernel+sizeof(int), descripcion_tabla, largo_descripcion);

					prot_enviar_mensaje(socket_k, POINT_DESCRIBE, tamanio_buffer_kernel, buffer_kernel);

					free(buffer_kernel);
					free(descripcion_tabla);
				}
				else if(data_del_fs->head == FAILED_DESCRIBE){
					prot_enviar_mensaje(socket_k, FAILED_DESCRIBE, 0, NULL);
				}

				free(buffer);
				prot_destruir_mensaje(data_del_fs);
			}
			else{
				prot_enviar_mensaje(socket_fs, DESCRIBE, 0, NULL);
				t_prot_mensaje* data_del_fs = prot_recibir_mensaje(socket_fs);

				if(data_del_fs->head == FULL_DESCRIBE){
					int largo_descripcion;
					char* descripcion_tabla;

					memcpy(&largo_descripcion, data_del_fs->payload, sizeof(int));
					descripcion_tabla = malloc(largo_descripcion+1);
					memcpy(descripcion_tabla, data_del_fs->payload+sizeof(int), largo_descripcion);
					descripcion_tabla[largo_descripcion]='\0';

					int tamanio_buffer_kernel = sizeof(int)+largo_descripcion;
					void* buffer_kernel = malloc(tamanio_buffer_kernel);

					memcpy(buffer_kernel, &largo_descripcion, sizeof(int));
					memcpy(buffer_kernel+sizeof(int), descripcion_tabla, largo_descripcion);

					prot_enviar_mensaje(socket_k, FULL_DESCRIBE, tamanio_buffer_kernel, buffer_kernel);

					free(buffer_kernel);
					free(descripcion_tabla);

				}
				else if(data_del_fs->head == FAILED_DESCRIBE){
					prot_enviar_mensaje(socket_k, FAILED_DESCRIBE, 0, NULL);
				}

				prot_destruir_mensaje(data_del_fs);
			}
		} break;

		case DESCONEXION:
		{
			printf("El Kernel Se ha desconectado\n");
		} break;

		case FALLO_AL_RECIBIR:
		{
			printf("Hubo un error re zarpado\n");
		} break;


		case GOSSIPING:
		{
			int tamanio_tabla = list_size(tabla_gossip);
			int tamanio_buffer = 0;

			for (int i=0; i<tamanio_tabla; i++)
			{
				t_est_memoria* memoria = list_get(tabla_gossip, i);
				int tamanio_memoria = strlen(memoria->ip_memoria)+sizeof(memoria->puerto_memoria)+2;
				tamanio_buffer += tamanio_memoria;
			}

			char* buffer = malloc(tamanio_buffer+1);
			bool primeraLectura = true;

			for (int i=0; i<tamanio_tabla; i++)
			{
				t_est_memoria* memoria= list_get(tabla_gossip, i);
				if(primeraLectura){
					strcpy(buffer, memoria->ip_memoria);
					primeraLectura = false;
				}
				else
				{
					strcat(buffer, memoria->ip_memoria);
				}

				strcat(buffer, ",");
				char* puerto = string_itoa(memoria->puerto_memoria);
				strcat(buffer, puerto);
				strcat(buffer, ";");
			}

			int tamanio_buffer_kernel = sizeof(int) + tamanio_buffer;

			void* buffer_kernel = malloc(tamanio_buffer_kernel);

			memcpy(buffer_kernel, &tamanio_buffer, sizeof(int));
			memcpy(buffer_kernel+sizeof(int), buffer, tamanio_buffer);
			prot_enviar_mensaje(socket_k, GOSSIPING, tamanio_buffer_kernel, buffer_kernel);

			usleep(info_memoria.retardo_mp*1000);

		}break;

		case JOURNAL_REQ:
		{
			pthread_mutex_lock(&mutex_estructuras_memoria);
			journalReq();
			pthread_mutex_unlock(&mutex_estructuras_memoria);

			usleep(info_memoria.retardo_mp*1000);

		} break;

		default:
		{
			printf("HEADER DESCONOCIDO\n");
		} break;

	}

	prot_destruir_mensaje(req_recibida);
	close(socket_k);
	printf("El Kernel se ha desconectado\n\n");
}
