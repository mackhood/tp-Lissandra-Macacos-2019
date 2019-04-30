/*
 * conexionMemoria.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "conexionMemoria.h"

void handler_conexion_memoria(t_kernel* tKernel) {


int conexion=conectar_a_servidor(tKernel->config->ip_memoria,tKernel->config->puerto_memoria,"Memoria");

	if (!conexion){

		logInfo("fallo conexion");

	}
	logInfo("Se conecto a una memoria");

	//prot_enviar_mensaje()



}
