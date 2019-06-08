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


socket_memoria = conectar_a_servidor(t_Criterios->strongConsistency->ip, t_Criterios->strongConsistency->puerto, "Memoria");

while(quantum >0 && dtb->flag != 1  && dtb->total_sentencias > 0 ) {

	params* parametros = malloc( sizeof(params) );
	inicializarParametros(parametros);


		inicializarParametros(parametros);

		char** args = string_split(dtb->tablaSentencias[dtb->sentenciaActual], " ");





		if(strcmp(args[0], "SELECT")==0)
		dtb->operacionActual= SELECT_REQ;

		if(strcmp(args[0], "INSERT")==0)
		dtb->operacionActual= INSERT_REQ;

		if(strcmp(args[0], "CREATE")==0)
		dtb->operacionActual= CREATE_REQ;

		if(strcmp(args[0], "DROP")==0)
		dtb->operacionActual= DROP_REQ;

		if(strcmp(args[0], "DESCRIBE_REQ")==0)
		dtb->operacionActual= DESCRIBE_REQ;


		dtb->total_sentencias--;

switch(dtb->operacionActual) {


	case SELECT_REQ : {

	//	if(estaEnMetadata(dtb->parametros->arreglo[0])){

		int largo_nombre_tabla = strlen(args[1]);
		size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla;
		void* buffer = malloc(tamanio_buffer);
		uint16_t key  = atoi(args[2]);
		memcpy(buffer,&largo_nombre_tabla , sizeof(int));
		memcpy(buffer+sizeof(int),args[1] , largo_nombre_tabla);
		memcpy(buffer+sizeof(int)+largo_nombre_tabla, &key, sizeof(u_int16_t));

		prot_enviar_mensaje(socket_memoria, SELECT_REQ, tamanio_buffer, buffer);

		t_prot_mensaje* req_recibida = prot_recibir_mensaje(socket_memoria);
		int largoValue;
		char * prueba;
		memcpy(&largoValue,req_recibida->payload,sizeof(int));
		prueba = malloc(largoValue + 1);
		memcpy(prueba,req_recibida->payload+sizeof(int),largoValue);
		prueba[largoValue]=	'\0';

		printf("el value solicitado es %s \n",prueba);





//		}else{
//
//		logInfo("no esta en metadata tabla");
//		dtb->flag=1;
//		}

//		char* nombre_tabla = string_duplicate(args[1]);
//		uint16_t key = atoi(args[2]);

		quantum--;
		dtb->sentenciaActual++;
	break;
	}
	case INSERT_REQ :{


		int largo_value = strlen(args[3]);
		int largo_nombre_tabla = strlen(args[1]);
		u_int16_t key = atoi(args[2]);
		size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla + sizeof(int) + largo_value;
		void* buffer = malloc(tamanio_buffer);
		memcpy(buffer,&largo_nombre_tabla,sizeof(int));
		memcpy(buffer + sizeof(int),args[1],largo_nombre_tabla);
		memcpy(buffer+sizeof(int)+largo_nombre_tabla,&key,sizeof(u_int16_t));
		memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(u_int16_t),&largo_value,sizeof(int));
		memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(u_int16_t)+sizeof(int),args[3],largo_value);



		prot_enviar_mensaje(socket_memoria, INSERT_REQ, tamanio_buffer, buffer);


		t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(socket_memoria);


		printf("Insert realizado \n");


		dtb->sentenciaActual++;





//
//



		break;
	}
	case CREATE_REQ :{



		//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]

						char* nombre_tabla = string_duplicate(args[1]);
						char* tipo_consistencia = string_duplicate(args[2]);
						int numero_particiones = atoi(args[3]);
						int compaction_time= atoi(args[4]);
						int largo_nombre_tabla= strlen(nombre_tabla);
						int largo_tipo_consistencia = strlen(tipo_consistencia);

						size_t tamanio_buffer = sizeof(int)+ largo_nombre_tabla +sizeof(int)+largo_tipo_consistencia+sizeof(int)+sizeof(int);
						void * buffer = malloc(tamanio_buffer);

						memcpy(buffer,&largo_nombre_tabla,sizeof(int));
						memcpy(buffer+sizeof(int),nombre_tabla,largo_nombre_tabla);
						memcpy(buffer+sizeof(int)+largo_nombre_tabla,&largo_tipo_consistencia,sizeof(int));
						memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(int),tipo_consistencia,largo_tipo_consistencia);
						memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia,&numero_particiones,sizeof(int));
						memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(int)+largo_tipo_consistencia+sizeof(int),&compaction_time,sizeof(int));


						prot_enviar_mensaje(socket_memoria, CREATE_REQ, tamanio_buffer, buffer);
						t_prot_mensaje * mensaje_recibido = prot_recibir_mensaje(socket_memoria);
						//printf("Insert realizado \n");
						dtb->sentenciaActual++;

						switch(mensaje_recibido->head){
						case TABLA_CREADA_OK:{
							printf("la tabla fue creada\n");
						}break;
						case TABLA_CREADA_YA_EXISTENTE:{
						printf("la tabla ya se encuentra existente\n");
						}break;
						case TABLA_CREADA_FALLO:{
							printf("hubo un error al crear la tabla\n");
						}break;
						default:{
							break;
						} 		}

		break;
	}
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
						dtb->sentenciaActual++;


		break;

	case DESCRIBE_REQ :
	{
		char* tablename = string_new();
		if(args[1] != NULL)
		{
			tablename = malloc(strlen(args[1]));
			strcpy(tablename, args[1]);
			int tamanio_tabla = strlen(tablename);
			size_t sizeOfBuffer = tamanio_tabla + sizeof(int);
			void* buffer = malloc(sizeOfBuffer);
			memcpy(buffer, &tamanio_tabla, sizeof(int));
			memcpy(buffer + sizeof(int), tablename, tamanio_tabla);
			prot_enviar_mensaje(socket_memoria, DESCRIBE_REQ, sizeOfBuffer, buffer);
		}
		else
		{
			prot_enviar_mensaje(socket_memoria, DESCRIBE_REQ, 0, NULL);
		}
		t_prot_mensaje* mensaje_memoria = prot_recibir_mensaje(socket_memoria);
		switch(mensaje_memoria->head)
		{
		case POINT_DESCRIBE:
		{
			int tamanio_mensaje;
			char* metadataTabla;
			memcpy(&tamanio_mensaje, mensaje_memoria->payload, sizeof(int));
			metadataTabla = malloc(tamanio_mensaje + 1);
			memcpy(metadataTabla, mensaje_memoria->payload + sizeof(int), tamanio_mensaje);
			metadataTabla[tamanio_mensaje] = '\0';
			break;
		}
		case FULL_DESCRIBE:
		{
			int tamanio_mensaje;
			char* metadataTablas;
			memcpy(&tamanio_mensaje, mensaje_memoria->payload, sizeof(int));
			metadataTablas = malloc(tamanio_mensaje + 1);
			memcpy(metadataTablas, mensaje_memoria->payload + sizeof(int), tamanio_mensaje);
			metadataTablas[tamanio_mensaje] = '\0';

			char ** tablas = string_split(metadataTablas,";");
			int m =0;
			while(tablas[m] != NULL ){

				char** free = string_split(tablas[m],",");
				if(list_any_satisfy(tMetadata->tablas,estaEnMetadata))
				list_add(tMetadata->tablas,free[0]);

				m++;

			}



			break;
		}
		case FAILED_DESCRIBE:
		{
			printf("No había tablas en el FS.\n");
			break;
		}
		default:
			break;
		}
		dtb->sentenciaActual++;
		break;
	}
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

