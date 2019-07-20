/*
 * Consola.c
 *
 *  Created on: 10 abr. 2019
 *      Author: utnso
 */


#include "Consola.h"

COMANDO comandos[] = {
		{"SELECT",selectt},
		{"INSERT",insert},
		{"CREATE",create},
		{"DESCRIBE",describe},
		{"DROP",drop},
		{"JOURNAL",journal},
		{"ADD",add},
		{"RUN",run},
		{"METRICS",metrics},
		{"MODIFYQUANTUM", modifyQuantum},
		{(char *) NULL, (Funcion *) NULL}
}; // para generalizar las funciones reciben un string.




void handleConsola(){


	puts("-_____________________________________________________");
	puts("CONSOLA");
	puts("------ Escriba un comando ------");
	puts("1. - SELECT  <tabla> <key>");
	puts("2. - INSERT <tabla> <key> <value> (value may contain spaces between each word)");
	puts("3. - CREATE <tabla>");
	puts("4. - DROP <tabla>");
	puts("5. - DESCRIBE <tabla> (puede dejarse vacia)");
	puts("6. - JOURNAL");
	puts("7. - ADD <Memoria> TO <Criterio>");
	puts("8. - RUN <Script>");
	puts("9. - METRICS");
	puts("10. - MODIFYQUANTUM <value>");
	char* linea;
	path_archivos_lql = "/home/utnso/workspace/tp-2019-1c-Macacos/Kernel/1C2019-Scripts-lql-entrega/scripts/";
	while (1)
	{

		linea = readline("\nIngrese el comando a ejecutar con los parámetros necesarios:\n ");
		char* aux = malloc(strlen(linea) + 1);
		strcpy(aux, linea);
		string_to_lower(aux);
		if (!strcmp(aux, "exit"))
		{
			free(linea);
			puts("El Kernel procederá a apagarse.");
			clear_history();
			free(aux);
			break;
		}
		if(!strcmp(linea, ""))
			puts("Por favor, ingrese un comando.");
		else if (ejecutar_linea(linea))
		{
			add_history(linea);
		}
		free(aux);
		free(linea);
	}
}

COMANDO * buscar_comando(char* linea) {
	int i;

	for (i = 0; comandos[i].nombre; i++){
		// Paso a miniscula y comparo
		string_to_lower(linea);
		char* comando = string_duplicate(comandos[i].nombre);
		string_to_lower(comando);

		if (strcmp (linea, comando) == 0){
			free(comando);
			return (&comandos[i]);
		}
		free(comando);

	}
	return ((COMANDO *) NULL);
}

int ejecutar_linea (char * linea){
	char * linea_aux = string_duplicate(linea);
	int i = 0;
	char * funcion;

	if(string_contains(linea, " "))
	{
		while (linea_aux[i] != ' ') i++;

		funcion = malloc((sizeof(char) * i) + 1);
		strncpy(funcion, linea_aux, i);
		funcion[i] = '\0';
	}
	else
	{
		funcion = string_duplicate(linea_aux);
	}


	COMANDO * comando = buscar_comando(funcion);

	if(!(comando != NULL))
	{
		puts("El comando que quiere ejecutar no coincide con ningún comando conocido.");
		free(funcion);
		free(linea_aux);
	}
	else if(doesNotContainNumber(comando->nombre))
	{
		char** args;
		args = string_split(linea_aux, " ");
		if (!comando) {
			printf ("%s: el comando ingresado es incorrecto.\n", funcion);
			return (-1);
		}

		free(funcion);

		//Llamo a la función
		(*(comando->funcion)) (args);
		if(!strcmp(comando->nombre, "INSERT"))
		{
			free(args[0]);
			free(args);
		}
		else
		{
			free(linea_aux);
			liberadorDeArrays(args);
		}
	}
	else
	{
		puts("El comando que quiere ejecutar no tiene el formato especificado.");
		free(funcion);
		free(linea_aux);
	}
	return 1;
}

void selectt(char** args){
	logInfo("[Consola]: Se ha recibido un pedido de select.");
	if(chequearParametros(args, 3) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError("[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(!itsANumber(args[2]))
		{
			puts("La key que ingresó posee caracteres invalidos.");
		}
		else
		{
			char* nombre_tabla = string_duplicate(args[1]);

			DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum);
			dtb_nuevo->total_sentencias =1;
			dtb_nuevo->quantum= tKernel->config->quantum;
			dtb_nuevo->total_sentencias=1;


			int b=0;
			char *unaPalabra = string_new();
			while( args[b] !=NULL){
				string_append(&args[b], " ");
				string_append(&unaPalabra, args[b]);
				b++;
			}

			queue_push(dtb_nuevo->tablaSentenciasMejorada,unaPalabra);
			//dtb_nuevo->tablaSentencias[0]=unaPalabra;


			if(estaEnMetadata(nombre_tabla)){

				enviarANew(dtb_nuevo);
				logInfo("Se envio a new el proceso");

			} else{

				enviarAEXIT(dtb_nuevo);
				logInfo("La tabla no se encuentra en metadata");

			}

			free(nombre_tabla);
		}
	}
}

//void inicializarParametros(params* params){
//
//
//	params->enteros[0]=0;
//	params->enteros[1]=0;
//	params->enteros[2]=0;
//	params->enteros[3]=0;
//	params->enteros[4]=0;
//	int i;
//	for(i=0; i<10; i++){
//
//		params->arreglo[i] = string_new();
//	}
//
//
//}

void insert (char** args)
{




	logInfo( "[Consola]: Se ha recibido un pedido de insert.");
	int chequeo = 0;
	int i = 0;
	for(i = 1 ; i < 4; i++)
	{
		if(args[i] == NULL)
		{
			chequeo = 1;
			break;
		}
		else
			chequeo = 0;
	}
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(!itsANumber(args[2]))
		{
			puts("La key que ingresó posee caracteres inválidos.");
			logError( "[Consola]: la key a Insertar es inválida.");
		}
		else if(strlen(args[2]) > 5)
		{
			puts("La key que ingresó es demasiado grande.");
			logError( "[Consola]: la key a Insertar es demasiado grande.");
		}
		else
		{
			// INSERT Tabla1|key|value
			char* nombre_tabla = string_duplicate(args[1]);
			char* claveaux = malloc(strlen(args[2]) + 1);
			strcpy(claveaux, args[2]);
			if(atoi(claveaux) > 65536)
			{
				puts("La key es demasiado grande, ingrese una más pequeña.");
				logError( "[Consola]: la key a Insertar es inválida.");
				free(nombre_tabla);
				free(claveaux);
				return;
			}
			else
			{

				int b =0;

				char *unaPalabra = string_new();
				while( args[b] !=NULL)
				{
					string_append(&args[b], " ");
					string_append(&unaPalabra, args[b]);
					b++;
				}

				DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum);
				dtb_nuevo->total_sentencias=1;
				queue_push(dtb_nuevo->tablaSentenciasMejorada,unaPalabra);

				//				dtb_nuevo->tablaSentencias[0]=unaPalabra;
				if(estaEnMetadata(nombre_tabla))
				{
					enviarANew(dtb_nuevo);
					logInfo("Se envio a new el proceso");
				}
				else
				{
					enviarAEXIT(dtb_nuevo);
					logInfo("La tabla no se encuentra en metadata");
				}

				free(nombre_tabla);
			}
		}
	}


}

void create (char** args)
{
	//				0		1					2					3
	//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
	//	Ej:
	//	CREATE TABLA1 SC 4 6000
	logInfo( "[Consola]: Se ha recibido un pedido de create.");
	if(chequearParametros(args, 5) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		if(criterioInvalido(args[2]))
		{
			puts("El criterio es inválido.");
			logError( "[Consola]: el criterio de CREATE es inválido.");
		}
		else if(!itsANumber(args[3]))
		{
			puts("La cantidad de particiones es inválida.");
			logError( "[Consola]: la cantidad de particiones para CREATE es inválida.");
		}
		else if(atoi(args[3]) < 1)
		{
			puts("La cantidad de particiones es inválida.");
			logError( "[Consola]: la cantidad de particiones para CREATE es inválida.");
		}
		else if(!itsANumber(args[4]))
		{
			puts("El tiempo entre cada compactación es inválido.");
			logError( "[Consola]: el tiempo entre cada compactación para CREATE es inválido.");
		}
		else
		{
			char* nombre_tabla = string_duplicate(args[1]);

			int b =0;
			char *unaPalabra = string_new();
			while( args[b] !=NULL)
			{
				string_append(&args[b], " ");
				string_append(&unaPalabra, args[b]);
				b++;
			}

			DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum);
			dtb_nuevo->total_sentencias=1;


			//			dtb_nuevo->tablaSentencias[0]=unaPalabra;
			queue_push(dtb_nuevo->tablaSentenciasMejorada,unaPalabra);

			enviarANew(dtb_nuevo);
			if(!estaEnMetadata(nombre_tabla))
			{
				enviarANew(dtb_nuevo);
				logInfo("Se envio a new el proceso");

			}
			else
			{
				enviarAEXIT(dtb_nuevo);
				logInfo("La tabla ya se encuentra en metadata");
			}
		}
	}
}

void describe (char** args)
{
	logInfo( "[Consola]: Se ha recibido un pedido de describe.");
	int chequeo = 0;
	if(args[1] == NULL )
		chequeo = chequearParametros(args, 1);
	else
		chequeo = chequearParametros(args, 2);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char *unaPalabra = string_new();
		int b =0;
		while( args[b] !=NULL)
		{
			string_append(&args[b], " ");
			string_append(&unaPalabra, args[b]);
			b++;
		}
		DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum);
		dtb_nuevo->total_sentencias=1;
		//		dtb_nuevo->tablaSentencias[0]=unaPalabra;
		queue_push(dtb_nuevo->tablaSentenciasMejorada,unaPalabra);


		if(args[1] !=NULL && estaEnMetadata(args[1])){

			enviarAEXIT(dtb_nuevo);
			logInfo("La tabla ya se encuentra en metadata");

		} else{
			enviarANew(dtb_nuevo);
			logInfo("Se envio a new el proceso");

		}
	}
}

void drop (char** args)
{
	logInfo( "[Consola]: Se ha solicitado realizar un DROP");
	if(chequearParametros(args, 2) == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* nombre_tabla = string_duplicate(args[1]);
		//char* key = string_duplicate(args[2]);

		//			params* parametros = malloc( sizeof(params) );
		//			inicializarParametros(parametros);
		//
		//			parametros->arreglo[0] = nombre_tabla;

		char *unaPalabra = string_new();
		int b =0;
		while( args[b] !=NULL)
		{
			string_append(&args[b], " ");
			string_append(&unaPalabra, args[b]);
			b++;
		}
		DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum);
		dtb_nuevo->total_sentencias = 1;
		//		dtb_nuevo->tablaSentencias[0]=unaPalabra;
		queue_push(dtb_nuevo->tablaSentenciasMejorada,unaPalabra);



		if(estaEnMetadata(nombre_tabla))
		{
			enviarANew(dtb_nuevo);
			logInfo("Se envio a new el proceso");
		}
		else
		{
			enviarAEXIT(dtb_nuevo);
			logInfo("La tabla no se encuentra en metadata");
		}
		free(nombre_tabla);
	}
}

void journal (char** args) {
	logInfo( "[Consola]: Se ha recibido un pedido de describe.");
	int chequeo = 0;
	chequeo = chequearParametros(args, 1);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		int x =0;
		while(NULL != list_get(tKernel->memoriasConCriterio,x)){


			memoria* laMemoria = list_get(tKernel->memoriasConCriterio,x);
			int socket_memoria = conectar_a_memoria_flexible(laMemoria->ip,laMemoria->puerto,KERNEL);
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

					while(conectar_a_memoria_flexible(loMemoria->ip,loMemoria->puerto,KERNEL) == -3){

						pthread_mutex_lock(&memoriasCola);
						memoria * loMemoria = queue_pop(tKernel->memoriasCola);
						pthread_mutex_unlock(&memoriasCola);
						if(loMemoria != NULL){



						}else{
							printf("no quedan memorias para asignar a SC");
							destProtocol = 1;
							printf ("VOLO TODO");

						}
					}


				}


			}else {


				prot_enviar_mensaje(socket_memoria, JOURNAL_REQ, 0, NULL);


				x++;

			}




		}

	}
}

void add (char** args) {

	logInfo( "[Consola]: Se ha recibido un pedido de Add.");
	int chequeo = 0;
	chequeo = chequearParametros(args, 4);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{

		//	int memoria = atoi(strdup(args[2]));
		//	char* criterio = strdup(args[4]);
		if(criterioInvalido(args[3]))
		{
			puts("El criterio es inválido.");
			logError( "[Consola]: el criterio de ADD es inválido.");
		}
		else if(!itsANumber(args[1]))
		{
			puts("El numero de memoria es inválido.");
			logError( "[Consola]: El numero de memoria para ADD es inválido.");
		}
		else if(atoi(args[1]) < 1)
		{
			puts("El numero de memoria es inválido.");
			logError( "[Consola]: El numero de memoria para ADD es inválido.");
		}
		else
		{
			int  numero_memoria =  atoi((args[1]));
			char* criterio = string_duplicate(args[3]);
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
							memoriaAgregar = list_remove_by_condition(tKernel->memoriasSinCriterio,(void*) estaEnLaLista);
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
						int otrolSocket = conectar_a_memoria_flexible(fruta->ip,fruta->puerto,KERNEL);
						prot_enviar_mensaje(otrolSocket,JOURNAL_REQ,0,NULL);
						close(otrolSocket);

						u++;
					}


					list_add(t_Criterios->StrongHash, memoriaAgregar);

					u=0;
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





		}
	}
}

unsigned long obtenerTamanioArchivo(char* direccionArchivo)
{
	FILE* FP = fopen(direccionArchivo, "r+");
	fseek(FP, 0, SEEK_END);
	unsigned long tamanio = (unsigned long)ftell(FP);
	fclose(FP);
	return tamanio;
}

void run (char** args) {
	logInfo( "[Consola]: Se ha recibido un pedido de Run.");
	int chequeo = 0;
	chequeo = chequearParametros(args, 2);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		char* path = string_duplicate(args[1]);
		char* absolute_path = string_duplicate(path_archivos_lql);
		string_append(&absolute_path, path);
		DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum);
		int x=0;
		FILE *fp;
		fp = fopen(absolute_path, "r"); // read mode
		if (fp == NULL)
		{
			perror("Error while opening the file.\n");
			exit(EXIT_FAILURE);
		}
		int size = obtenerTamanioArchivo(absolute_path) ;
		char* sentenciasParsear = (char *) mmap (0, size, PROT_READ, MAP_SHARED, fp->_fileno, 0);
		char** argus = string_split(sentenciasParsear,"\n");
		int b=0;
		while( argus[b] !=NULL)
		{
			queue_push(dtb_nuevo->tablaSentenciasMejorada,argus[b]);
			b++;
		}
		x=b;
		dtb_nuevo->total_sentencias = x;
		enviarANew(dtb_nuevo);
		printf("se envio a new el proceso");
	}
}

void metrics (char ** args) {
	logInfo( "[Consola]: Se ha recibido un pedido de Metrics.");
	int chequeo = 0;
	chequeo = chequearParametros(args, 1);
	if(chequeo == 1)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError( "[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else
	{
		int i;
		int tiempoReadsTotalEC = 0;
		int tiempoReadsTotalSC = 0;
		int tiempoReadsTotalSHC = 0;
		int tiempoWritesTotalEC = 0;
		int tiempoWritesTotalSC = 0;
		int tiempoWritesTotalSHC = 0;
		int writesTotalEC = 0;
		int writesTotalSC = 0;
		int writesTotalSHC = 0;
		int readsTotalEC = 0;
		int readsTotalSC = 0;
		int readsTotalSHC = 0;
		for( i=0 ; i< list_size(tKernel->memoriasConCriterio);i++)
		{
			memoria* unaMemoria =	list_get(tKernel->memoriasConCriterio,i);
			tiempoReadsTotalSC += unaMemoria->estadisticasMemoriaSC->Read_Latency;
			readsTotalSC += unaMemoria->estadisticasMemoriaSC->Reads;
			tiempoWritesTotalSC += unaMemoria->estadisticasMemoriaSC->Write_Latency;
			writesTotalSC += unaMemoria->estadisticasMemoriaSC->Writes;
			tiempoReadsTotalSHC += unaMemoria->estadisticasMemoriaSHC->Read_Latency;
			readsTotalSHC += unaMemoria->estadisticasMemoriaSHC->Reads;
			tiempoWritesTotalSHC += unaMemoria->estadisticasMemoriaSHC->Write_Latency;
			writesTotalSHC += unaMemoria->estadisticasMemoriaSHC->Writes;
			tiempoReadsTotalEC += unaMemoria->estadisticasMemoriaEC->Read_Latency;
			readsTotalEC += unaMemoria->estadisticasMemoriaEC->Reads;
			tiempoWritesTotalEC += unaMemoria->estadisticasMemoriaEC->Write_Latency;
			writesTotalEC += unaMemoria->estadisticasMemoriaEC->Writes;
		}
		logInfo("Estadísticas de las memorias del criterio SC:");
		puts("Estadísticas de las memorias del criterio SC:");
		logInfo("La cantidad de lecturas de SC fue: %s", string_itoa(readsTotalSC));
		printf("La cantidad de lecturas de SC fue: %s\n", string_itoa(readsTotalSC));
		if(readsTotalSC > 0)
		{
			logInfo("El tiempo promedio de cada lectura de SC fue: %s", string_itoa(tiempoReadsTotalSC/readsTotalSC));
			printf("El tiempo promedio de cada lectura de SC fue: %s\n", string_itoa(tiempoReadsTotalSC/readsTotalSC));
		}
		else
		{
			logInfo("No hubo lecturas de SC, por ende no hay tiempo que calcular");
			printf("No hubo lecturas de SC, por ende no hay tiempo que calcular.\n");
		}
		logInfo("La cantidad de escrituras de SC fue: %s",string_itoa(writesTotalSC));
		printf("La cantidad de escrituras de SC fue: %s\n",string_itoa(writesTotalSC));
		if(writesTotalSC > 0)
		{
			logInfo("El tiempo promedio de cada escritura de SC fue: %s", string_itoa(tiempoReadsTotalSC/writesTotalSC));
			printf("El tiempo promedio de cada escritura de SC fue: %s\n", string_itoa(tiempoReadsTotalSC/writesTotalSC));
		}
		else
		{
			logInfo("No hubo escrituras de SC, por ende no hay tiempo que calcular");
		}
		logInfo("Estadísticas de las memorias del criterio SHC:");
		puts("Estadísticas de las memorias del criterio SHC:");
		logInfo("La cantidad de lecturas de SHC fue: %s", string_itoa(readsTotalSHC));
		printf("La cantidad de lecturas de SHC fue: %s\n", string_itoa(readsTotalSHC));
		if(readsTotalSHC > 0)
		{
			logInfo("El tiempo promedio de cada lectura de SHC fue: %s", string_itoa(tiempoReadsTotalSHC/readsTotalSHC));
			printf("El tiempo promedio de cada lectura de SHC fue: %s\n", string_itoa(tiempoReadsTotalSHC/readsTotalSHC));
		}
		else
		{
			logInfo("No hubo lecturas de SHC, por ende no hay tiempo que calcular");
			printf("No hubo lecturas de SHC, por ende no hay tiempo que calcular.\n");
		}
		logInfo("La cantidad de escrituras de SHC fue: %s",string_itoa(writesTotalSHC));
		printf("La cantidad de escrituras de SHC fue: %s\n",string_itoa(writesTotalSHC));
		if(writesTotalSHC > 0)
		{
			logInfo("El tiempo promedio de cada escritura de SHC fue: %s", string_itoa(tiempoWritesTotalSHC/writesTotalSHC));
			printf("El tiempo promedio de cada escritura de SHC fue: %s\n", string_itoa(tiempoWritesTotalSHC/writesTotalSHC));
		}
		else
		{
			logInfo("No hubo escrituras de SHC, por ende no hay tiempo que calcular");
		}
		logInfo("Estadísticas de las memorias del criterio EC:");
		puts("Estadísticas de las memorias del criterio EC:");
		logInfo("La cantidad de lecturas de EC fue: %s", string_itoa(readsTotalEC));
		printf("La cantidad de lecturas de EC fue: %s\n", string_itoa(readsTotalEC));
		if(readsTotalEC > 0)
		{
			logInfo("El tiempo promedio de cada lectura de EC fue: %s", string_itoa(tiempoReadsTotalEC/readsTotalEC));
			printf("El tiempo promedio de cada lectura de EC fue: %s\n", string_itoa(tiempoReadsTotalEC/readsTotalEC));
		}
		else
		{
			logInfo("No hubo lecturas de EC, por ende no hay tiempo que calcular");
			printf("No hubo lecturas de EC, por ende no hay tiempo que calcular.\n");
		}
		logInfo("La cantidad de escrituras de EC fue: %s",string_itoa(writesTotalEC));
		printf("La cantidad de escrituras de EC fue: %s\n",string_itoa(writesTotalEC));
		if(writesTotalEC > 0)
		{
			logInfo("El tiempo promedio de cada escritura de EC fue: %s", string_itoa(tiempoWritesTotalEC/writesTotalEC));
			printf("El tiempo promedio de cada escritura de EC fue: %s\n", string_itoa(tiempoWritesTotalEC/writesTotalEC));
		}
		else
		{
			logInfo("No hubo escrituras de EC, por ende no hay tiempo que calcular");
		}
		puts("\n A continuacion, el Memory Load de cada una de las memorias que se encuentran conectadas");
		logInfo("A continuacion, el Memory Load de cada una de las memorias que se encuentran conectadas");
		int r;
		for( r = 0 ; r< list_size(tKernel->memoriasConCriterio);r++)
		{
			memoria* unaMemoria =	list_get(tKernel->memoriasConCriterio,r);
			int totalRequestsRespondidas = unaMemoria->estadisticasMemoriaEC->Reads + unaMemoria->estadisticasMemoriaEC->Writes + unaMemoria->estadisticasMemoriaSC->Reads + unaMemoria->estadisticasMemoriaSC->Writes + unaMemoria->estadisticasMemoriaSHC->Reads + unaMemoria->estadisticasMemoriaSHC->Writes;
			if(totalRequestsRespondidas > 0)
			{
				printf("La memoria %d se encargó del %d (porciento) de las requests\n", unaMemoria->numeroMemoria,
						(totalRequestsRespondidas/(t_estadisticas->Reads + t_estadisticas->Writes))*100);
				logInfo("La memoria %d se encargó del %d (porciento) de las requests\n", unaMemoria->numeroMemoria,
						(totalRequestsRespondidas/(t_estadisticas->Reads + t_estadisticas->Writes))*100);
			}
			else
			{
				printf("La memoria %d no recibió ninguna request", unaMemoria->numeroMemoria);
				logInfo("La memoria %d no recibió ninguna request", unaMemoria->numeroMemoria);
			}
		}
	}
}

void modifyQuantum(char** args)
{
	logInfo("[Consola]: Ha llegado un pedido para modificar el quantum entre instrucciones.");
	int chequeo = chequearParametros(args, 2);
	if(chequeo)
	{
		printf("Por favor, especifique la cantidad de parámetros solicitada.\n");
		logError("[Consola]: solicitud posee cantidad errónea de parámetros");
	}
	else if(!itsANumber(args[1]))
	{
		puts("El valor que ingresó tiene caracteres inválidos.");
		logError( "[Consola]: el valor para modificar es inválido.");
	}
	else
	{
		t_config* ConfigMain = config_create(PATH_CONFIG);
		config_set_value(ConfigMain, "QUANTUM", args[1]);
		config_save(ConfigMain);
		config_destroy(ConfigMain);
		logInfo("[Consola]: se ha modificado el quantum");
		puts("Se ha modificado el quantum");
	}
}

