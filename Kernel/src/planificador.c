/*
 * planificador.c
 *
 *  Created on: 5 jun. 2019
 *      Author: utnso
 */


#include "planificador.h"



void pasarArunnign() {

	while(1){


		if(!queue_is_empty(tKernelEstados->new)){


			DTB_KERNEL* dtb=(DTB_KERNEL*)getDTBNew();
			enviarAReady(dtb);

		}


		if(tKernel->config->multiprocesamiento > 0 && !queue_is_empty(tKernelEstados->ready) ){

			DTB_KERNEL* dtb=(DTB_KERNEL*)getDTBReady();
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

int socket_memoria = 0;

	case SELECT_REQ :


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

//		char* nombre_tabla = string_duplicate(args[1]);
//		uint16_t key = atoi(args[2]);

		quantum--;
		dtb->sentenciaActual++;
	break;

	case INSERT_REQ :



		break;

	case CREATE_REQ :



		break;

	case DROP_REQ :



		break;

	case DESCRIBE_REQ :



		break;


		default :

			printf("Invalid operation\n" );

}



}

}

