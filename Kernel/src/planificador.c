/*
 * planificador.c
 *
 *  Created on: 5 jun. 2019
 *      Author: utnso
 */


#include "planificador.h"



void pasarArunnign() {

	while(1){


//		if(!queue_is_empty(tKernelEstados->new)){
//
//
//			DTB_KERNEL* dtb=(DTB_KERNEL*)getDTBNew();
//			enviarAReady(dtb);
//
//		}




			DTB_KERNEL* dtb=(DTB_KERNEL*)getDTBReady();
			if(tKernel->config->multiprocesamiento > 0  ){
			dtb->quantum = tKernel->config->quantum;
			enviarAEjecutar(dtb);
			tKernel->config->multiprocesamiento --;
			pthread_t threadProceso;

			pthread_create(&threadProceso, NULL, (void*)ejecutarProceso,dtb);
			pthread_detach(threadProceso);


		}






	}


}






void ejecutarProceso(DTB_KERNEL* dtb){

int quantum = dtb->quantum;
dtb->sentenciaActual=0;
int socket_memoria = 0;

while(quantum >0 && dtb->flag != 1  && dtb->total_sentencias > 0 ) {

	params* parametros = malloc( sizeof(params) );
	inicializarParametros(parametros);


		inicializarParametros(parametros);
		char** args = string_split(dtb->tablaSentencias[dtb->sentenciaActual], " ");


		if(strcmp(args[0], "SELECT")==0)
		dtb->operacionActual= SELECT_REQ;

		if(strcmp(args[0], "INSERT")==0)
		dtb->operacionActual= INSERT_REQ;

		if(strcmp(args[0], "CREATE_REQ")==0)
		dtb->operacionActual= CREATE_REQ;

		if(strcmp(args[0], "DROP_REQ")==0)
		dtb->operacionActual= INSERT_REQ;

		if(strcmp(args[0], "DESCRIBE_REQ")==0)
		dtb->operacionActual= DESCRIBE_REQ;


		dtb->total_sentencias--;
switch(dtb->operacionActual) {


	case SELECT_REQ :

		if(estaEnMetadata(dtb->parametros->arreglo[0])){
		socket_memoria = conectar_a_servidor(t_Criterios->strongConsistency->ip, t_Criterios->strongConsistency->puerto, "Memoria");

		int largo_nombre_tabla = strlen(dtb->parametros->arreglo[0]);
		size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla;
		void* buffer = malloc(tamanio_buffer);
		uint16_t key  = (uint16_t)dtb->parametros->enteros[0];
		memcpy(buffer,&key , sizeof(uint16_t));
		memcpy(buffer+sizeof(uint16_t), &largo_nombre_tabla, sizeof(int));
		memcpy(buffer+sizeof(uint16_t)+sizeof(int), dtb->parametros->arreglo[0], largo_nombre_tabla);

		prot_enviar_mensaje(socket_memoria, SELECT_REQ, tamanio_buffer, buffer);



		t_prot_mensaje* req_recibida = prot_recibir_mensaje(socket_memoria);
		printf(req_recibida->payload);

		}else{

		logInfo("no esta en metadata tabla");
		dtb->flag=1;
		}

//		char* nombre_tabla = string_duplicate(args[1]);
//		uint16_t key = atoi(args[2]);

		quantum--;
		dtb->sentenciaActual++;
	break;

	case INSERT_REQ :




//					int tamanio_nombre_tabla;
//					char* nombre_tabla;
//					uint16_t key;
//					int tamanio_value;
//					char* value;
//
//					memcpy(&tamanio_nombre_tabla, req_recibida->payload, sizeof(int));
//					nombre_tabla = malloc(tamanio_nombre_tabla+1);
//					memcpy(nombre_tabla, req_recibida->payload + sizeof(int), tamanio_nombre_tabla);
//					nombre_tabla[tamanio_nombre_tabla] = '\0';
//					memcpy(&key, req_recibida->payload + sizeof(int) + tamanio_nombre_tabla, sizeof(uint16_t));
//					memcpy(&tamanio_value, req_recibida->payload +sizeof(int)+tamanio_nombre_tabla+sizeof(uint16_t), sizeof(int));
//					value = malloc(tamanio_value+1);
//					memcpy(value, req_recibida->payload+sizeof(int)+tamanio_nombre_tabla+sizeof(uint16_t), tamanio_value);
//					value[tamanio_value] = '\0';





		break;

	case CREATE_REQ :

//		int largo_nombre_tabla;
//						char* nombre_tabla;
//						int largo_tipo_consistencia;
//						char* tipo_consistencia;
//						int numero_particiones;
//						int compaction_time;
//
//						memcpy(&largo_nombre_tabla, req_recibida->payload, sizeof(int));
//						nombre_tabla = malloc(largo_nombre_tabla+1);
//						memcpy(nombre_tabla, req_recibida->payload+sizeof(int), largo_nombre_tabla);
//						nombre_tabla[largo_nombre_tabla]='\0';
//						memcpy(&largo_tipo_consistencia, req_recibida->payload+sizeof(int)+largo_nombre_tabla, sizeof(int));
//						tipo_consistencia = malloc(largo_tipo_consistencia+1);
//						memcpy(tipo_consistencia, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int), largo_tipo_consistencia);
//						memcpy(&numero_particiones, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia, sizeof(int));
//						memcpy(&compaction_time, req_recibida->payload+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia+sizeof(int), sizeof(int));


		break;

	case DROP_REQ :

//		int largo_nombre_tabla;
//						char* nombre_tabla;
//
//						memcpy(&largo_nombre_tabla, req_recibida->payload, sizeof(int));
//						nombre_tabla = malloc(largo_nombre_tabla + 1);
//						memcpy(nombre_tabla, req_recibida->payload+sizeof(int), largo_nombre_tabla);
//						nombre_tabla[largo_nombre_tabla] = '\0';
//
//						dropReq(nombre_tabla);
//
//						int tamanio_buffer = sizeof(int)+largo_nombre_tabla;
//						void* buffer = malloc(tamanio_buffer);
//						memcpy(buffer, &largo_nombre_tabla, sizeof(int));
//						memcpy(buffer + sizeof(int), nombre_tabla, largo_nombre_tabla);
//
//						//mando solicitud de drop de tabla al FS
//						prot_enviar_mensaje(socket_fs, TABLE_DROP, tamanio_buffer, buffer);
//						t_prot_mensaje* respuesta = prot_recibir_mensaje(socket_fs);


		break;

	case DESCRIBE_REQ :



		break;


		default :

		printf("Invalid operation\n" );

				}



				}

				if(dtb->flag){
					logInfo("Fallo , se suspende todo");
					moverExecToExit(dtb);

					close(socket_memoria);


				}else{

				if(dtb->total_sentencias == 0){

					logInfo("Finalizo");
					moverExecToExit(dtb);
					close(socket_memoria);

				}else{

					logInfo("Fin de quantum");
					moverExecToReady(dtb);
					close(socket_memoria);

				}


				}


}

