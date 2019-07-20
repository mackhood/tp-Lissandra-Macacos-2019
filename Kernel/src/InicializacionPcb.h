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
void destruir_dtb(DTB_KERNEL* pcb);
void initNuevoGDT(char* pathScriptEscriptorio);
void liberarDTB(DTB_KERNEL* dtb);


/********** Definir variables globales **********/



#endif /* INICIALIZACIONPCB_H_ */
