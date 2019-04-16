/*
 * main.c
 *
 *  Created on: 1 abr. 2019
 *      Author: utnso
 */


#include <stdio.h>
#include <stdlib.h>
#include "main.h"



int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	initConfiguracion();

	//--------> German, la linea de abajo es la que indica como te vas a tener que conectar a la Memoria, no se de donde sacar los datos de tu config debido a que me dice que se encuentra undeclared
	//int socket_memoria = conectar_a_servidor(tKernel->config->ip_memoria, tKernel->config->puerto_memoria, "Kernel");

	initThread();


	return EXIT_SUCCESS;
}


















