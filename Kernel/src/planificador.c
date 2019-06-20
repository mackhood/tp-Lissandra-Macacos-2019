/*
 * planificador.c
 *
 *  Created on: 5 jun. 2019
 *      Author: utnso
 */


#include "planificador.h"



void pasarArunnign() {

	while(1){







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





void actualizarEstadisticas(){





}






void ejecutarProceso(DTB_KERNEL* dtb){

	int quantum = dtb->quantum;
	dtb->sentenciaActual=0;
	int socket_memoria = 0;






	socket_memoria = conectar_a_servidor(t_Criterios->strongConsistency->ip, t_Criterios->strongConsistency->puerto, "Memoria");

	while(quantum >0 && dtb->flag != 1  && dtb->total_sentencias > 0 ) {



		//	SELECT
		//	INSERT
		//	CREATE
		//	DESCRIBE
		//	DROP










		//	params* parametros = malloc( sizeof(params) );
		//	inicializarParametros(parametros);
		//
		//
		//		inicializarParametros(parametros);



		char** args = string_split(dtb->tablaSentencias[dtb->sentenciaActual], " ");





		if(strcmp(args[0], "SELECT")==0)
			dtb->operacionActual= SELECT_REQ;

		if(strcmp(args[0], "INSERT")==0)
			dtb->operacionActual= INSERT_REQ;

		if(strcmp(args[0], "CREATE")==0)
			dtb->operacionActual= CREATE_REQ;

		if(strcmp(args[0], "DROP")==0)
			dtb->operacionActual= DROP_REQ;

		if(strcmp(args[0], "DESCRIBE")==0)
			dtb->operacionActual= DESCRIBE_REQ;

		if(strcmp(args[0], "JOURNAL")==0)
			dtb->operacionActual= JOURNAL_REQ;

		if(strcmp(args[0], "ADD")==0)
			dtb->operacionActual= ADD_REQ;


		dtb->total_sentencias--;


		//		if(dtb->operacionActual ==SELECT_REQ || dtb->operacionActual == INSERT_REQ || dtb->operacionActual == CREATE_REQ || dtb->operacionActual == DROP_REQ || dtb->operacionActual ==DESCRIBE_REQ)
		//		{
		//
		//			if(args[1]== NULL ){
		//
		//			memoria * memoriaConectar  =queue_pop(tKernel->memoriasCola);
		//
		//		socket_memoria = conectar_a_servidor(memoriaConectar->ip, memoriaConectar->puerto, "KERNEL");
		//
		//
		//
		//			}
		//			else{
		//
		//				//
		//
		//
		//
		//
		//
		//
		//			}


		//	}




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
			int value_total =0;
			int c =3;
			while(args[c] != NULL ){

				value_total +=	strlen(args[c]);
				c++;
				value_total++;
			}

			char *stringEnviar = string_new();


			int d=3;
			while(args[d] != NULL ){

				if(args[d+1] !=NULL ){
					string_append(&stringEnviar, strcat(args[d], " "));
				}

				string_append(&stringEnviar,args[d]);

				d++;

			}


			int largoStringEnviar =strlen(stringEnviar);

			size_t tamanio_buffer = sizeof(uint16_t) + sizeof(int) + largo_nombre_tabla + sizeof(int) + value_total;
			void* buffer = malloc(tamanio_buffer);
			memcpy(buffer,&largo_nombre_tabla,sizeof(int));
			memcpy(buffer + sizeof(int),args[1],largo_nombre_tabla);
			memcpy(buffer+sizeof(int)+largo_nombre_tabla,&key,sizeof(u_int16_t));
			memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(u_int16_t),&largoStringEnviar,sizeof(int));
			memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(u_int16_t)+sizeof(int),stringEnviar,largoStringEnviar);

			//		int i =3;
			//		while(args[i] != NULL){
			//			int largo_value=0;
			//			largo_value = strlen(args[i]) +1;
			//			memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(u_int16_t),&largo_value,sizeof(int));
			//
			//			memcpy(buffer+sizeof(int)+largo_nombre_tabla+sizeof(u_int16_t)+sizeof(int),args[i],largo_value);
			//			i++;
			//		}

			double cantSegundosInicial= getCurrentTime() ;
			prot_enviar_mensaje(socket_memoria, INSERT_REQ, tamanio_buffer, buffer);


			t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(socket_memoria);
			double cantSegundosFinal= getCurrentTime() ;



			printf("Insert realizado \n");


			dtb->sentenciaActual++;

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

				bool  estaEnMetadata(void* una_tabla) {
					return  string_equals_ignore_case((char *) nombre_tabla,(char*)una_tabla);
				}


				if(!list_any_satisfy(tMetadata->tablas,(void*)estaEnMetadata)){

					t_tabla* nuevaTabla = malloc(sizeof(t_tabla));
					nuevaTabla->criterio = tipo_consistencia;
					nuevaTabla->nombre= nombre_tabla;

					list_add(tMetadata->tablas,nuevaTabla);

					printf("la tabla fue creada\n");

				}
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
		case DROP_REQ :{

			char* nombre_tabla = strdup(args[1]);
			int largo_nombre_tabla = strlen(nombre_tabla);

			int tamanio_buffer = sizeof(int)+largo_nombre_tabla;
			void* buffer = malloc(tamanio_buffer);
			memcpy(buffer, &largo_nombre_tabla, sizeof(int));
			memcpy(buffer + sizeof(int), nombre_tabla, largo_nombre_tabla);

			//mando solicitud de drop de tabla a Memoria
			prot_enviar_mensaje(socket_memoria, DROP_REQ, tamanio_buffer, buffer);
			t_prot_mensaje* respuesta = prot_recibir_mensaje(socket_memoria);
			dtb->sentenciaActual++;

			//posibles respuestas
			switch(respuesta->head){
			case TABLE_DROP_OK:{
				printf("La tabla fue borrada\n");


				bool  estaEnMetadata(t_tabla* una_tabla) {
					return  string_equals_ignore_case((char *) nombre_tabla,una_tabla->nombre);
				}


				list_remove_by_condition(tMetadata->tablas,(void*)estaEnMetadata);

			}break;
			case TABLE_DROP_NO_EXISTE:{
				printf("La tabla no existe\n");
			}break;
			case TABLE_DROP_FALLO:{
				printf("hubo un error\n");
			}break;
			default:{
				break;
			}
			}

			break;
		}

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
				printf(metadataTabla);
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



				char ** tablaDescribe = string_split(metadataTablas, ";");

				int a=0;
				list_clean(tMetadata->tablas);
				while(tablaDescribe[a] != NULL  ){





					char* tabla = strtok(tablaDescribe[a],",");
					char* criterio = strtok(NULL, ",");
					a++;
					t_tabla * nuevaTabla = malloc(sizeof(t_tabla));
					nuevaTabla->nombre = tabla;
					nuevaTabla->criterio = criterio;

					bool  estaEnMetadata(t_tabla* tablaAux) {
						return  string_equals_ignore_case(nuevaTabla->nombre, tablaAux->nombre);
					}


					if(!list_any_satisfy(tMetadata->tablas,(void*)estaEnMetadata))
						list_add(tMetadata->tablas, nuevaTabla);


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

		case ADD_REQ:
		{

			int  numero_memoria =  atoi((args[2]));
			char* criterio = string_duplicate(args[4]);


			if(!strcmp(criterio,"SC")){

				if (t_Criterios->strongConsistency->numeroMemoria != numero_memoria  && t_Criterios->strongConsistency != NULL )
				{



					bool  estaEnLaLista(memoria* memoriaAux) {
						return  numero_memoria == memoriaAux->numeroMemoria;
					}



					memoria* memoriaAgregar ;

					if(list_find(tKernel->memoriasSinCriterio,(void*)estaEnLaLista)!= NULL ){

						pthread_mutex_lock(&memoriasSinCriterio);
						memoriaAgregar =	list_remove_by_condition(tKernel->memoriasSinCriterio,(void*) estaEnLaLista);
						pthread_mutex_unlock(&memoriasSinCriterio);
						pthread_mutex_lock(&memoriasCriterio);
						list_add(tKernel->memoriasConCriterio,memoriaAgregar);
						pthread_mutex_unlock(&memoriasCriterio);


					}else {

						memoriaAgregar =list_find(tKernel->memoriasConCriterio,(void*)estaEnLaLista);


					}


					memoria* masMemoria;
				bool estaEnSC(memoria* memoriaAux2) {
					return  numero_memoria == memoriaAux2->numeroMemoria;
				}

				if(list_find(tKernel->memoriasConCriterio,(void*)estaEnSC)!= NULL ){

					pthread_mutex_lock(&memoriasSinCriterio);
					masMemoria =list_remove_by_condition(tKernel->memoriasSinCriterio,(void*) estaEnSC);
					pthread_mutex_unlock(&memoriasSinCriterio);
					pthread_mutex_lock(&memoriasCriterio);
					list_add(tKernel->memoriasConCriterio,masMemoria);
					pthread_mutex_unlock(&memoriasCriterio);


				}
				}







			}




		}





		break;

		}

		case JOURNAL_REQ:
		{





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

