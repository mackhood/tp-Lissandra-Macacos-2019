/*
 * main.c
 *
 *  Created on: 1 abr. 2019
 *      Author: utnso
 */



#include "main.h"



int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	initConfiguracion();

	initThread();

	pthread_mutex_t stop;
	pthread_mutex_init(&stop, NULL);
	pthread_mutex_lock(&stop);
	pthread_mutex_lock(&stop);
	return EXIT_SUCCESS;
}

