/*
 * Consola.c
 *
 *  Created on: 10 abr. 2019
 *      Author: utnso
 */


#include "Consola.h"








COMANDO comandos[] = {
		{"select",selectt},
		{"insert",insert},
		{"create",create},
		{"DESCRIBE",describe},
		{"drop",drop},
		{"journal",journal},
		{"add",add},
		{"run",run},
		{"metrics",metrics},
		{(char *) NULL, (Funcion *) NULL}
}; // para generalizar las funciones reciben un string.




void handleConsola(){


	puts("-_____________________________________________________");
	puts("CONSOLA");
	puts("------ Escriba un comando ------");
	puts("1. - SELECT  <ruta-al-escritorio>");
	puts("2. - INSERT  <ruta-al-escritorio>");
	puts("3. - CREATE  <ruta-al-escritorio>");
	puts("4. - DROP  <ruta-al-escritorio>");
	puts("5. - JOURNAL  <ruta-al-escritorio>");
	puts("6. - ADD    <id_dtb>");
	puts("7. - DROP  <ruta-al-escritorio>");
	puts("8. - RUN <ruta-al-escritorio>");
	puts("9. - METRICS  <id_dtb>");
	char* linea;

	while (1) {
		linea = readline("\n Kernel: ");

		char* aux = malloc(strlen(linea) + 1);
		strcpy(aux, linea);
		//string_to_upper(aux);



		if (strcmp(aux, "EXIT")==0){
			free(linea);
			free(aux);
			destProtocol = 1;
			puts("El Kernel ha sido desconectado.");
			break;
		}

		if (ejecutar_linea(aux)){
			add_history(aux);
		}
		//free(linea);
		free(aux);
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

	if(string_contains(linea, "|"))
	{
		while (linea_aux[i] != '|') i++;

		funcion = malloc((sizeof(char) * i) + 1);
		strncpy(funcion, linea_aux, i);
		funcion[i] = '\0';
	}
	else
	{
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
	}

	COMANDO * comando = buscar_comando(funcion);

	char** args;
	if(!strcmp(comando->nombre, "INSERT"))
	{
		args = string_split(linea_aux, "|");
	}
	else
		args = string_split(linea_aux, " ");
	if (!comando) {
		printf ("%s: el comando ingresado es incorrecto.", funcion);
		return (-1);
	}

	free(funcion);
	free(linea_aux);
	//Llamo a la función
	(*(comando->funcion)) (args);
	free(args[0]);
	free(args[1]);

	free(args);
	return 1;
}





void selectt(char** args){
	char* nombre_tabla = string_duplicate(args[1]);
	uint16_t key = atoi(args[2]);
	//char* key = string_duplicate(args[2]);

	params* parametros = malloc( sizeof(params) );
	inicializarParametros(parametros);
	parametros->enteros[0]= key;
	parametros->arreglo[0] = nombre_tabla;



	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,parametros);
	dtb_nuevo->total_sentencias =1;
	dtb_nuevo->quantum= tKernel->config->quantum;
	dtb_nuevo->total_sentencias=1;

	//	if(estaEnMetadata(nombre_tabla)){
	//
	//		enviarANew(dtb_nuevo);
	//		logInfo("Se envio a new el proceso");
	//
	//	} else{
	//
	//		enviarAEXIT(dtb_nuevo);
	//		logInfo("La tabla no se encuentra en metadata");
	//
	//	}


	char* tabla [100];
	int i;
	int b=0;
	for(i=0; i<100; i++){

		tabla[i] = string_new();


	}



	char *unaPalabra = string_new();
	while( args[b] !=NULL){



		string_append(&unaPalabra, strcat(args[b], " "));
		b++;
	}

	dtb_nuevo->tablaSentencias[0]=unaPalabra;

	enviarANew(dtb_nuevo);
	logInfo("Se envio a new el proceso");

	free(nombre_tabla);

}

void inicializarParametros(params* params){


	params->enteros[0]=0;
	params->enteros[1]=0;
	params->enteros[2]=0;
	params->enteros[3]=0;
	params->enteros[4]=0;
	int i;
	for(i=0; i<10; i++){

		params->arreglo[i] = string_new();
	}


}


void insert (char** args) {

	// INSERT Tabla1|key|value
//	char* nombre_tabla = string_duplicate(args[1]);
//	uint16_t key = atoi(args[2]);
//	char* value = string_duplicate(args[3]);
//
//	//char* key = string_duplicate(args[2]);
//
//	params* parametros = malloc( sizeof(params) );
//	inicializarParametros(parametros);
//
//	parametros->enteros[0]= key;
//	parametros->arreglo[0] = nombre_tabla;
//	parametros->arreglo[1] = value;

	int b =0;

	char *unaPalabra = string_new();
	while( args[b] !=NULL){



		string_append(&unaPalabra, strcat(args[b], " "));
		b++;
	}



	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,NULL);
	dtb_nuevo->total_sentencias=1;
	dtb_nuevo->tablaSentencias[0]=unaPalabra;
	//		if(estaEnMetadata(nombre_tabla)){
	//
	//			enviarANew(dtb_nuevo);
	//			logInfo("Se envio a new el proceso");
	//
	//		} else{
	//
	//			enviarAEXIT(dtb_nuevo);
	//			logInfo("La tabla no se encuentra en metadata");
	//
	//		}
	char* tabla [100];

	enviarANew(dtb_nuevo);

	//free(nombre_tabla);


}
void create (char** args) {
	//		0		1		2					3					4
	//	CREATE [TABLA] [TIPO_CONSISTENCIA] [NUMERO_PARTICIONES] [COMPACTION_TIME]
	//	Ej:
	//	CREATE TABLA1 SC 4 60000

	char* nombre_tabla = string_duplicate(args[1]);
	int numeroParticiones = atoi(args[3]);
	char* tipoConsistencia = string_duplicate(args[2]);
	int tiempoCompactacion = atoi(args[4]);
	//char* key = string_duplicate(args[2]);

	params* parametros = malloc( sizeof(params) );
	inicializarParametros(parametros);
	parametros->enteros[0]= numeroParticiones;
	parametros->enteros[1]=tiempoCompactacion;
	parametros->arreglo[0] = nombre_tabla;
	parametros->arreglo[1] = tipoConsistencia;

	int b =0;
	char *unaPalabra = string_new();
	while( args[b] !=NULL){



		string_append(&unaPalabra, strcat(args[b], " "));
		b++;
	}








	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,parametros);
	dtb_nuevo->total_sentencias=1;


	dtb_nuevo->tablaSentencias[0]=unaPalabra;
	enviarANew(dtb_nuevo);
	//					if(estaEnMetadata(nombre_tabla)){
	//
	//						enviarANew(dtb_nuevo);
	//						logInfo("Se envio a new el proceso");
	//
	//					} else{
	//
	//						enviarAEXIT(dtb_nuevo);
	//						logInfo("La tabla no se encuentra en metadata");
	//
	//					}

}


void describe (char** args) {




	char *unaPalabra = string_new();
	int b =0;
	while( args[b] !=NULL){



		string_append(&unaPalabra, strcat(args[b], " "));
		b++;


	}



	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,NULL);
	dtb_nuevo->total_sentencias=1;
	dtb_nuevo->tablaSentencias[0]=unaPalabra;
	enviarANew(dtb_nuevo);





}
void drop (char** args) {


	char* nombre_tabla = string_duplicate(args[1]);

	//char* key = string_duplicate(args[2]);

	//			params* parametros = malloc( sizeof(params) );
	//			inicializarParametros(parametros);
	//
	//			parametros->arreglo[0] = nombre_tabla;



	char *unaPalabra = string_new();
	int b =0;
	while( args[b] !=NULL){



		string_append(&unaPalabra, strcat(args[b], " "));
		b++;


	}



	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,NULL);
	dtb_nuevo->total_sentencias=1;
	dtb_nuevo->tablaSentencias[0]=unaPalabra;
	enviarANew(dtb_nuevo);


	//			if(estaEnMetadata(nombre_tabla)){
	//
	//				enviarANew(dtb_nuevo);
	//				logInfo("Se envio a new el proceso");
	//
	//			} else{
	//
	//				enviarAEXIT(dtb_nuevo);
	//				logInfo("La tabla no se encuentra en metadata");
	//
	//			}



	free(nombre_tabla);




}
void journal (char** args) {


	//	char *unaPalabra = string_new();
	//	int b =0;
	//	while( args[b] !=NULL){
	//
	//
	//
	//	string_append(&unaPalabra, strcat(args[b], " "));
	//	b++;
	//
	//
	//		}
	//
	//	//send journal to all memories in tKernel->memoriasConCriterio; Implementar en planificador
	//
	//	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,NULL);
	//				dtb_nuevo->total_sentencias=1;
	//				dtb_nuevo->tablaSentencias[0]=unaPalabra;
	//				enviarANew(dtb_nuevo);

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

}
void add (char** args) {


//		int memoria = atoi(strdup(args[2]));
//		char* criterio = strdup(args[4]);
//
//
//
//	char *unaPalabra = string_new();
//	int b =0;
//	while( args[b] !=NULL){
//
//
//
//		string_append(&unaPalabra, strcat(args[b], " "));
//		b++;
//
//
//	}
//
//	//send journal to all memories in tKernel->memoriasConCriterio; Implementar en planificador
//
//	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,NULL);
//	dtb_nuevo->total_sentencias=1;
//	dtb_nuevo->tablaSentencias[0]=unaPalabra;
//	enviarANew(dtb_nuevo);
////



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

	char* path = string_duplicate(args[1]);

	params* parametros = malloc( sizeof(params) );
	inicializarParametros(parametros);

	parametros->arreglo[0] = path;

	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel->config->quantum,parametros);



	//char linea[1024];
	//    FILE *fich;
	//
	//    fich = fopen(path, "r");
	//    while(fgets(linea, 1024, (FILE*) fich)) {
	//        printf("LINEA: %s FIN_DE_LINEA\n", linea);
	//    }
	//    fclose(fich);
	//}


	char* tabla [100];
	int i;
	int x=0;
	int a =0;
	for(i=0; i<100; i++){

		tabla[i] = string_new();


	}

	char * ch =string_new();
	FILE *fp;

	//printf("Enter name of a file you wish to see\n");


	fp = fopen(path, "r"); // read mode

	if (fp == NULL)
	{
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}

	//printf("The contents of %s file are:\n", path);





	//   char auxiliar [100];
	//   int f =0;
	//   char outs= fgetc(fp);
	//   while(!feof(fp)){
	//
	//	   switch(outs){
	//
	//	   case '\n':{
	//
	//		   memcpy(tabla[x], auxiliar,f);
	//		   x++;
	//		   f=0;
	//		   break;
	//	   }
	//	   default: {
	//
	//		   auxiliar[f]=outs;
	//		   f++;
	//
	//		   break;
	//	   }
	//	   }
	//
	//	   outs= fgetc(fp);
	//   }
	//   	   auxiliar[f] = '\0';
	//   	   f++;
	//   	   memcpy(tabla[x], auxiliar,f);
	//



	int size = obtenerTamanioArchivo(path) ;




	char* sentenciasParsear = (char *) mmap (0, size, PROT_READ, MAP_SHARED, fp->_fileno, 0);


	char** argus = string_split(sentenciasParsear,"\n");



	int b=0;

	while( argus[b] !=NULL){



		dtb_nuevo->tablaSentencias[b]=argus[b];

		b++;
	}

	x=b;
	//   for(b=0;b<=x;b++){
	//
	//
	//	 dtb_nuevo->tablaSentencias[b]=tabla[b];
	//
	//   }
	dtb_nuevo->total_sentencias = x;
	//  dtb_nuevo->sentenciaActual=a;


	enviarANew(dtb_nuevo);
	// logInfo("Se envio a new el proceso");
	printf("se envio a new el proceso");





}

void metrics (char ** args) {




	logInfo("Estadisticas globales de todos los criterios");
	logInfo(string_itoa(t_estadisticas->Reads));
	logInfo(string_itoa(t_estadisticas ->Read_Latency));
	logInfo(string_itoa(t_estadisticas->Write_Latency));
	logInfo(string_itoa(t_estadisticas->Writes));
	int i;
	for( i=0 ; i< list_size(tKernel->memorias);i++){

		memoria* unaMemoria =	list_get(tKernel->memoriasConCriterio,i);
		logInfo("Memoria Numero : %d ", unaMemoria->numeroMemoria);

		logInfo("Criterio SC:");

		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Read_Latency));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Reads));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Write_Latency));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Writes));

		if(unaMemoria->selectTotales == 0){


			logInfo("Memory Load para SC de esta memoria es 0");

		}else{


			logInfo("Memory Load para SC de esta memoria es : %f", unaMemoria->insertsTotales / unaMemoria->selectTotales);



		}


		logInfo("Criterio SHC:");



		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Read_Latency));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Reads));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Write_Latency));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Writes));

		if(unaMemoria->selectTotales == 0){


			logInfo("Memory Load para SHC de esta memoria es 0");

		}else{


			logInfo("Memory Load para SHC de esta memoria es : %f", unaMemoria->insertsTotales / unaMemoria->selectTotales);



		}




		logInfo("Criterio EC:");





		logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Read_Latency));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Reads));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Write_Latency));
		logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Writes));

		if(unaMemoria->selectTotales == 0){


			logInfo("Memory Load para EC de esta memoria es 0");

		}else{


			logInfo("Memory Load para EC de esta memoria es : %f", unaMemoria->insertsTotales / unaMemoria->selectTotales);



		}


	}


}

//void ejecutarScript(char** args){
//
//	if(args[1] !=NULL){
//		char* parametro = malloc( (sizeof(char)*string_length(args[1])) + 1);
//		memcpy(parametro, args[1], string_length(args[1]) + 1);
//		logInfo("Operacion ejecutar %s", parametro);
//		//char* arg = string_duplicate(parametro);
//		initNuevoGDT(parametro);
//		free(parametro);
//	} else {
//		puts("Debe ingresar un parametro para la funcion Ejecutar");
//	}
//}
//
//void statusColas(char** arg){
//	char * parametro = arg[1];
//	if(parametro==NULL) {
//		mostrarStatusTodo();
//	} else {
//		mostrarStatus(atoi(parametro));
//	}
//	logInfo("Operacion status %s", parametro);
//}
//
//void finalizarDTB(char** arg){
//	int pid = atoi(arg[1]);
//	logInfo("Operacion finalizar %d", pid);
//	finalizarById(pid);
//}
//
//void metricaDTB(char** arg){
//	logInfo("Operacion opcional metrica %s", arg[1]);
//
//	char* parametro = arg[1];
//	if(parametro==NULL)
//		mostrarMetricaTodo();
//	else
//		mostrarMetrica( atoi(parametro) );
//}














