/*
 * planificador.c
 *
 *  Created on: 5 jun. 2019
 *      Author: utnso
 */


#include "planificador.h"



void pasarArunnign() {

	while(!destProtocol){







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






	//	socket_memoria = conectar_a_servidor(t_Criterios->strongConsistency->ip, t_Criterios->strongConsistency->puerto, "Memoria");

	while(dtb->quantum >0 && dtb->flag != 1  && dtb->total_sentencias > 0 ) {



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

		t_tabla* laTabla;
		memoria * leMemoria;
		if(dtb->operacionActual ==SELECT_REQ || dtb->operacionActual == INSERT_REQ || dtb->operacionActual == CREATE_REQ || dtb->operacionActual == DROP_REQ )
		{

			if((dtb->operacionActual == DESCRIBE_REQ && args[1] != NULL) ||dtb->operacionActual == DROP_REQ ){

				char * tablaABuscar = strdup(args[1]);
				bool  estaEnMetadata(t_tabla* una_tabla) {
					return  string_equals_ignore_case((char *) tablaABuscar,una_tabla->nombre);
				}
				laTabla = list_find(tMetadata->tablas,(void*)estaEnMetadata);

				if(laTabla != NULL){


					if(!strcmp(laTabla->criterio,"SC")){

						pthread_mutex_lock(&t_Criterios->strongConsistency->enUso);
						socket_memoria = conectar_a_memoria_flexible(t_Criterios->strongConsistency->ip, t_Criterios->strongConsistency->puerto, "Kernel");


					}else if(!strcmp(laTabla->criterio,"EC")){

						pthread_mutex_lock(&ec);
						leMemoria = queue_pop(t_Criterios->eventualConsistency);
						pthread_mutex_unlock(&ec);

						pthread_mutex_lock(&leMemoria->enUso);
						socket_memoria = conectar_a_memoria_flexible(leMemoria->ip, leMemoria->puerto, "Kernel");


					}else if(!strcmp(laTabla->criterio,"SHC")){

						int cantidadEnHash = list_size(t_Criterios->StrongHash);

						int value =	(rand() % cantidadEnHash);

						leMemoria = list_get(t_Criterios->StrongHash,value);
						pthread_mutex_lock(&leMemoria->enUso);
						socket_memoria = conectar_a_memoria_flexible(leMemoria->ip, leMemoria->puerto, "Kernel");


					} else{


						dtb->operacionActual = ERROR;
						printf("No se encontro criterio");
						dtb->flag = 1;

					}




				}	dtb->operacionActual = ERROR;
				printf("La tabla no se encuentra en metadata");
				dtb->flag = 1;

			}else{
				if(dtb->operacionActual == DESCRIBE_REQ) {
					pthread_mutex_lock(&memoriasCola);
					leMemoria  =queue_pop(tKernel->memoriasCola);
					pthread_mutex_unlock(&memoriasCola);

					pthread_mutex_unlock(&leMemoria->enUso);
					socket_memoria = conectar_a_servidor(leMemoria->ip, leMemoria->puerto, "KERNEL");


				}else if (dtb->operacionActual == CREATE_REQ){


					if(!strcmp(args[2],"SC")){

						pthread_mutex_lock(&t_Criterios->strongConsistency->enUso);
						socket_memoria = conectar_a_memoria_flexible(t_Criterios->strongConsistency->ip, t_Criterios->strongConsistency->puerto, "Kernel");


					}else if(!strcmp(args[2],"EC")){

						pthread_mutex_lock(&ec);
						leMemoria = queue_pop(t_Criterios->eventualConsistency);
						pthread_mutex_unlock(&ec);

						pthread_mutex_lock(&leMemoria->enUso);
						socket_memoria = conectar_a_memoria_flexible(leMemoria->ip, leMemoria->puerto, "Kernel");


					}else if(!strcmp(args[2],"SHC")){

						int cantidadEnHash = list_size(t_Criterios->StrongHash);

						int value =	(rand() % cantidadEnHash);

						leMemoria = list_get(t_Criterios->StrongHash,value);
						pthread_mutex_lock(&leMemoria->enUso);
						socket_memoria = conectar_a_memoria_flexible(leMemoria->ip, leMemoria->puerto, "Kernel");


					} else{

						dtb->operacionActual = ERROR;
						printf("No se encontro criterio");
						dtb->flag = 1;

					}





				} else if(dtb->operacionActual == SELECT_REQ || dtb->operacionActual == INSERT_REQ){




					char * tablaABuscar = strdup(args[1]);
					bool  estaEnMetadata(t_tabla* una_tabla) {
						return  string_equals_ignore_case((char *) tablaABuscar,una_tabla->nombre);
					}
					laTabla = list_find(tMetadata->tablas,(void*)estaEnMetadata);

					if(laTabla != NULL){


						if(!strcmp(laTabla->criterio,"SC")){

							pthread_mutex_lock(&t_Criterios->strongConsistency->enUso);
							socket_memoria = conectar_a_memoria_flexible(t_Criterios->strongConsistency->ip, t_Criterios->strongConsistency->puerto, "Kernel");


						}else if(!strcmp(laTabla->criterio,"EC")){

							pthread_mutex_lock(&ec);
							leMemoria = queue_pop(t_Criterios->eventualConsistency);
							pthread_mutex_unlock(&ec);

							pthread_mutex_lock(&leMemoria->enUso);
							socket_memoria = conectar_a_memoria_flexible(leMemoria->ip, leMemoria->puerto, "Kernel");


						}else if(!strcmp(laTabla->criterio,"SHC")){

							int cantidadEnHash = list_size(t_Criterios->StrongHash);

							int key =	atoi(args[2]);

							int value = key % cantidadEnHash;


							leMemoria = list_get(t_Criterios->StrongHash,value);
							pthread_mutex_lock(&leMemoria->enUso);
							socket_memoria = conectar_a_memoria_flexible(leMemoria->ip, leMemoria->puerto, "Kernel");


						} else{


							dtb->operacionActual = ERROR;
							printf("No se encontro criterio");
							dtb->flag = 1;

						}




					}	dtb->operacionActual = ERROR;
					printf("La tabla no se encuentra en metadata");
					dtb->flag = 1;






				}






			}


		}




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


			if(laTabla->criterio == "EC"){

				queue_push(t_Criterios->eventualConsistency,leMemoria);


			}





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

			if(laTabla->criterio == "EC"){

				queue_push(t_Criterios->eventualConsistency,leMemoria);


			}


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

			if(laTabla->criterio == "EC"){

				queue_push(t_Criterios->eventualConsistency,leMemoria);


			}

			//printf("Insert realizado \n");
			dtb->sentenciaActual++;

			switch(mensaje_recibido->head){
			case TABLA_CREADA_OK:{

				bool  estaEnMetadata(t_tabla* una_tabla) {
					return  string_equals_ignore_case((char *) nombre_tabla,una_tabla->nombre);
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
			if(laTabla->criterio == "EC"){

				queue_push(t_Criterios->eventualConsistency,leMemoria);


			}
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
			if(laTabla->criterio == "EC"){

				queue_push(t_Criterios->eventualConsistency,leMemoria);


			}

			if(args[1]!= NULL){

			}else{

				queue_push(tKernel->memoriasCola,leMemoria);

			}



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
			if(!strcmp(criterio,"SC"))
			{
				if(t_Criterios->strongConsistency != NULL)
				{
					if(t_Criterios->strongConsistency->numeroMemoria != numero_memoria)
					{
						bool  estaEnLaLista(memoria* memoriaAux) {
							return  numero_memoria == memoriaAux->numeroMemoria;
						}

						memoria* memoriaAgregar ;
						if(list_find(tKernel->memoriasSinCriterio,(void*)estaEnLaLista)!= NULL )
						{
							pthread_mutex_lock(&memoriasSinCriterio);
							memoriaAgregar =	list_remove_by_condition(tKernel->memoriasSinCriterio,(void*) estaEnLaLista);
							pthread_mutex_unlock(&memoriasSinCriterio);
							pthread_mutex_lock(&memoriasCriterio);
							list_add(tKernel->memoriasConCriterio,memoriaAgregar);
							pthread_mutex_unlock(&memoriasCriterio);
						}
						else
						{
							memoriaAgregar =list_find(tKernel->memoriasConCriterio,(void*)estaEnLaLista);
						}
						memoria* memoriaAReemplazar;
						bool estaLaDeSC(memoria* memoriaAux2) {
							return  t_Criterios->strongConsistency->numeroMemoria == memoriaAux2->numeroMemoria;
						}
						if(!list_any_satisfy(t_Criterios->StrongHash,(void*)estaLaDeSC) &&
								!list_any_satisfy(t_Criterios->eventualConsistency->elements,(void*) estaLaDeSC))
						{
							pthread_mutex_lock(&memoriasCriterio);
							memoriaAReemplazar =list_remove_by_condition(tKernel->memoriasConCriterio,(void*) estaLaDeSC);
							pthread_mutex_unlock(&memoriasCriterio);
							pthread_mutex_lock(&memoriasSinCriterio);
							list_add(tKernel->memoriasSinCriterio,memoriaAReemplazar);
							pthread_mutex_unlock(&memoriasSinCriterio);

						}
						pthread_mutex_lock(&sc);
						t_Criterios->strongConsistency = memoriaAgregar;
						pthread_mutex_unlock(&sc);
					}
					else
					{
						printf("Esta es la misma memoria que está asignada");
					}
				}
				else
				{
					bool  estaEnLaLista(memoria* memoriaAux) {
						return  numero_memoria == memoriaAux->numeroMemoria;
					}

					memoria* memoriaAgregar ;
					if(list_find(tKernel->memoriasSinCriterio,(void*)estaEnLaLista)!= NULL )
					{
						pthread_mutex_lock(&memoriasSinCriterio);
						memoriaAgregar =	list_remove_by_condition(tKernel->memoriasSinCriterio,(void*) estaEnLaLista);
						pthread_mutex_unlock(&memoriasSinCriterio);
						pthread_mutex_lock(&memoriasCriterio);
						list_add(tKernel->memoriasConCriterio,memoriaAgregar);
						pthread_mutex_unlock(&memoriasCriterio);
					}
					else
					{
						memoriaAgregar =list_find(tKernel->memoriasConCriterio,(void*)estaEnLaLista);
					}
					pthread_mutex_lock(&sc);
					t_Criterios->strongConsistency = memoriaAgregar;
					pthread_mutex_unlock(&sc);
				}
			}
			else if(!strcmp(criterio,"SHC"))
			{
				bool  estaEnLaLista(memoria* memoriaAux) {
					return  numero_memoria == memoriaAux->numeroMemoria;
				}
				if(NULL != list_find(t_Criterios->StrongHash, (void*)estaEnLaLista))
				{
					printf("La memoria ya está en este criterio");
				}
				else
				{
					memoria* memoriaAgregar ;
					if(list_find(tKernel->memoriasSinCriterio,(void*)estaEnLaLista) != NULL )
					{
						pthread_mutex_lock(&memoriasSinCriterio);
						memoriaAgregar = list_remove_by_condition(tKernel->memoriasSinCriterio,(void*) estaEnLaLista);
						pthread_mutex_unlock(&memoriasSinCriterio);
						pthread_mutex_lock(&memoriasCriterio);
						list_add(tKernel->memoriasConCriterio,memoriaAgregar);
						pthread_mutex_unlock(&memoriasCriterio);
					}
					else
					{
						memoriaAgregar = list_find(tKernel->memoriasConCriterio,(void*)estaEnLaLista);
					}
					int u= 0;
					while(list_get(t_Criterios->StrongHash,u) != NULL){

						memoria* fruta =list_get(t_Criterios->StrongHash,u);
						pthread_mutex_lock(&fruta->enUso);
						int otrolSocket = conectar_a_memoria_flexible(fruta->ip,fruta->puerto,"Kernel");
						prot_enviar_mensaje(otrolSocket,JOURNAL_REQ,0,NULL);
						close(otrolSocket);

						u++;
					}

					pthread_mutex_lock(&memoriaAgregar->enUso);
					list_add(t_Criterios->StrongHash, memoriaAgregar);

					u=0;
					while(list_get(t_Criterios->StrongHash,u) != NULL){

						memoria* fruta =list_get(t_Criterios->StrongHash,u);
						pthread_mutex_unlock(&fruta->enUso);


						u++;
					}



				}
			}
			else if(!strcmp(criterio,"EC"))
			{
				bool  estaEnLaLista(memoria* memoriaAux) {
					return  numero_memoria == memoriaAux->numeroMemoria;
				}
				if(NULL != list_find(t_Criterios->eventualConsistency->elements, (void*)estaEnLaLista))
				{
					printf("La memoria ya está en este criterio");
				}
				else
				{
					memoria* memoriaAgregar ;
					if(list_find(tKernel->memoriasSinCriterio,(void*)estaEnLaLista) != NULL )
					{
						pthread_mutex_lock(&memoriasSinCriterio);
						memoriaAgregar = list_remove_by_condition(tKernel->memoriasSinCriterio,(void*) estaEnLaLista);
						pthread_mutex_unlock(&memoriasSinCriterio);
						pthread_mutex_lock(&memoriasCriterio);
						list_add(tKernel->memoriasConCriterio,memoriaAgregar);
						pthread_mutex_unlock(&memoriasCriterio);
					}
					else
					{
						memoriaAgregar = list_find(tKernel->memoriasConCriterio,(void*)estaEnLaLista);
					}
					pthread_mutex_lock(&ec);
					queue_push(t_Criterios->eventualConsistency, memoriaAgregar);
					pthread_mutex_unlock(&ec);
				}
			}
			else
			{
				printf("El criterio es invalido");
			}
			break;
		}

		case JOURNAL_REQ:
		{



			int cantidadMemorias = list_size(tKernel->memoriasConCriterio);
			int x =0;
			bool hayMemorias = 1;
			while(x<cantidadMemorias && hayMemorias){


				memoria* laMemoria = list_get(tKernel->memoriasConCriterio,x);
				int socket_memoria = conectar_a_memoria_flexible(laMemoria->ip,laMemoria->puerto,"Kernel");
				if(socket_memoria == -3 ){

					bool  estaEnLaLista(memoria* memoriaAux) {
						return  laMemoria->numeroMemoria == memoriaAux->numeroMemoria;
					}
					pthread_mutex_lock(&memoriasCriterio);
					list_remove_by_condition(tKernel->memoriasConCriterio,(void*)estaEnLaLista);
					pthread_mutex_unlock(&memoriasCriterio);
					pthread_mutex_lock(&shc);
					list_remove_by_condition(t_Criterios->StrongHash,(void*)estaEnLaLista);
					pthread_mutex_unlock(&shc);
					pthread_mutex_lock(&ec);
					list_remove_by_condition(t_Criterios->eventualConsistency->elements,(void*)estaEnLaLista);
					pthread_mutex_unlock(&ec);
					if(t_Criterios->strongConsistency->numeroMemoria == laMemoria->numeroMemoria){

						pthread_mutex_lock(&memoriasCola);
						memoria * loMemoria = queue_pop(tKernel->memoriasCola);
						pthread_mutex_unlock(&memoriasCola);

						while(conectar_a_memoria_flexible(loMemoria->ip,loMemoria->puerto,"Kernel") == -3){

							pthread_mutex_lock(&memoriasCola);
							memoria * loMemoria = queue_pop(tKernel->memoriasCola);
							pthread_mutex_unlock(&memoriasCola);
							if(loMemoria != NULL){



							}else{
								printf("no quedan memorias para asignar a SC");
								hayMemorias = 0;
								destProtocol = 1;
								printf ("VOLO TODO");

							}
						}


					}


				}else {


					prot_enviar_mensaje(socket_memoria, JOURNAL_REQ, 0, NULL);




				}



				x++;
			}


			break;

		}


		case ERROR: {



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






