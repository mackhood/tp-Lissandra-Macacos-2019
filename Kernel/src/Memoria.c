/*
 * Memoria.c
 *
 *  Created on: 2 may. 2019
 *      Author: utnso
 */


#include "Memoria.h"



int actualIdMemoria = 0 ;

	int getIdMemoria(){
			int id;
			pthread_mutex_lock(&mutexIdMemoria);
			id = actualIdMemoria++;
			pthread_mutex_unlock(&mutexIdMemoria);
			return id;
		}


memoria* crearMemoria(int conexion){

	memoria* nuevaMemoria = malloc(sizeof(memoria));
	nuevaMemoria->conexion = conexion;
	nuevaMemoria->numeroMemoria = getIdMemoria();
	return nuevaMemoria;
}
