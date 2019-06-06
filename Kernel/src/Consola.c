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
		{"Read Latency",readLatency},
		{"Write Latency",writeLatency},
		{"Reads",reads},
		{"Writes",writes},
		{"Memory Load",memoryLoad},
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

		if (strcmp(linea, "exit")==0){
			free(linea);
			puts("EXIT.");
			break;
		}

		if (ejecutar_linea(linea)){
			add_history(linea);
		}
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



	DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel,parametros);

	if(estaEnMetadata(nombre_tabla)){

		enviarANew(dtb_nuevo);
		logInfo("Se envio a new el proceso");

	} else{

		enviarAEXIT(dtb_nuevo);
		logInfo("La tabla no se encuentra en metadata");

	}



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


	char* nombre_tabla = string_duplicate(args[1]);
		uint16_t key = atoi(args[2]);
		char* value = string_duplicate(args[1]);

		//char* key = string_duplicate(args[2]);

		params* parametros = malloc( sizeof(params) );
		inicializarParametros(parametros);

		parametros->enteros[0]= key;
		parametros->arreglo[0] = nombre_tabla;
		parametros->arreglo[1] = value;


		DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel,parametros);

		if(estaEnMetadata(nombre_tabla)){

			enviarANew(dtb_nuevo);
			logInfo("Se envio a new el proceso");

		} else{

			enviarAEXIT(dtb_nuevo);
			logInfo("La tabla no se encuentra en metadata");

		}



		free(nombre_tabla);


}
void create (char** args) {





}
void describe (char** args) {





}
void drop (char** args) {


	char* nombre_tabla = string_duplicate(args[1]);

			//char* key = string_duplicate(args[2]);

			params* parametros = malloc( sizeof(params) );
			inicializarParametros(parametros);

			parametros->arreglo[0] = nombre_tabla;


			DTB_KERNEL*  dtb_nuevo =(DTB_KERNEL*) crearDTBKernel(getIdGDT(),NULL,tKernel,parametros);

			if(estaEnMetadata(nombre_tabla)){

				enviarANew(dtb_nuevo);
				logInfo("Se envio a new el proceso");

			} else{

				enviarAEXIT(dtb_nuevo);
				logInfo("La tabla no se encuentra en metadata");

			}



			free(nombre_tabla);




}
void journal (char** args) {





}
void add (char** args) {





}
void run (char** args) {





}
void readLatency (char** args) {





}
void writeLatency (char** args) {





}
void reads (char** args) {





}
void writes (char** args) {





}
void memoryLoad (char** args) {





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














