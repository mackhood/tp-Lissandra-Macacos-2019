/*
 * InicializacionPcb.c
 *
 *  Created on: 30 abr. 2019
 *      Author: utnso
 */


#include "InicializacionPcb.h"

int actualIdGDT = 100;

int getIdGDT()
{
	int id;
	pthread_mutex_lock(&mutexIdGDT);
	id = actualIdGDT++;
	pthread_mutex_unlock(&mutexIdGDT);
	return id;
}

void destruir_dtb(DTB_KERNEL* pcb)
{
	free(pcb);
}

void initNuevoGDT(char* pathScriptEscriptorio){

	int idGdt = getIdGDT();
	agregarGDT( crearGdt(idGdt) );
}


