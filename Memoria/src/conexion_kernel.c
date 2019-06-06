#include "conexion_kernel.h"

void escucharKernel(int* kernel){
	int s_kernel = *kernel;
	free(kernel);

	while(1){

		t_prot_mensaje* req_recibida = prot_recibir_mensaje(s_kernel);

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

				prot_enviar_mensaje(socket_kernel, KEY_SOLICITADA_SELECT, strlen(value_solicitado), value_solicitado);

				free(nombre_tabla);

			} break;

			case INSERT_REQ:
			{
				int tamanio_nombre_tabla;
				char* nombre_tabla;
				uint16_t key;
				int tamanio_value;
				char* value;

				memcpy(&tamanio_nombre_tabla, req_recibida->payload, sizeof(int));
				nombre_tabla = malloc(tamanio_nombre_tabla+1);
				memcpy(nombre_tabla, req_recibida->payload + sizeof(int), tamanio_nombre_tabla);
				nombre_tabla[tamanio_nombre_tabla] = '\0';
				memcpy(&key, req_recibida->payload + sizeof(int) + tamanio_nombre_tabla, sizeof(uint16_t));
				memcpy(&tamanio_value, req_recibida->payload +sizeof(int)+tamanio_nombre_tabla+sizeof(uint16_t), sizeof(int));
				value = malloc(tamanio_value+1);
				memcpy(value, req_recibida->payload+sizeof(int)+tamanio_nombre_tabla+sizeof(uint16_t), tamanio_value);
				value[tamanio_value] = '\0';

				double time_insertado = insertReq(nombre_tabla, key, value);
				prot_enviar_mensaje(socket_kernel, INSERT_REALIZADO, 0, NULL);

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
				memcpy(&numero_particiones, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia, sizeof(int));
				memcpy(&compaction_time, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia+sizeof(int), sizeof(int));

				t_prot_mensaje* mensaje_fs = createReq(nombre_tabla, largo_nombre_tabla, tipo_consistencia, largo_tipo_consistencia, numero_particiones, compaction_time);

				switch(mensaje_fs->head){
							case TABLA_CREADA_OK:{
								prot_enviar_mensaje(socket_kernel, TABLA_CREADA_OK, 0, NULL);
							}break;
							case TABLA_CREADA_YA_EXISTENTE:{
								prot_enviar_mensaje(socket_kernel, TABLA_CREADA_YA_EXISTENTE, 0, NULL);
							}break;
							case TABLA_CREADA_FALLO:{
								prot_enviar_mensaje(socket_kernel, TABLA_CREADA_FALLO, 0, NULL);
							}break;
							default:{
								break;
							}
						}

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

				dropReq(nombre_tabla);

				int tamanio_buffer = sizeof(int)+largo_nombre_tabla;
				void* buffer = malloc(tamanio_buffer);
				memcpy(buffer, &largo_nombre_tabla, sizeof(int));
				memcpy(buffer + sizeof(int), nombre_tabla, largo_nombre_tabla);

				//mando solicitud de drop de tabla al FS
				prot_enviar_mensaje(socket_fs, TABLE_DROP, tamanio_buffer, buffer);
				t_prot_mensaje* respuesta = prot_recibir_mensaje(socket_fs);

				switch(respuesta->head){
					case TABLE_DROP_OK:{
						prot_enviar_mensaje(socket_kernel, TABLE_DROP_OK, 0, NULL);
					}break;
					case TABLE_DROP_NO_EXISTE:{
						prot_enviar_mensaje(socket_kernel, TABLE_DROP_NO_EXISTE, 0, NULL);
					}break;
					case TABLE_DROP_FALLO:{
						prot_enviar_mensaje(socket_kernel, TABLE_DROP_FALLO, 0, NULL);;
					}break;
					default:{
						break;
					}
				}

				free(nombre_tabla);
				prot_destruir_mensaje(respuesta);

			} break;

			default:
			{
				printf("HEADER DESCONOCIDO\n");
			}break;

		}
		prot_destruir_mensaje(req_recibida);
	}

}
