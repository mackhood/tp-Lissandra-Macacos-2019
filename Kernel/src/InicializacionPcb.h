/*
 * InicializacionPcb.h
 *
 *  Created on: 30 abr. 2019
 *      Author: utnso
 */

#ifndef INICIALIZACIONPCB_H_
#define INICIALIZACIONPCB_H_

#include "kernelConfig.h"

#include <pthread.h>
int getIdGDT();
void initConfigPLP();


/********** Definir variables globales **********/
pthread_mutex_t mutexIdGDT;


#endif /* INICIALIZACIONPCB_H_ */
