/*
 * Kernel.h
 *
 *  Created on: 2 abr. 2019
 *      Author: utnso
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>

#include <commons/config.h>
#include <commons/log.h>
#include "kernelConfig.h"
#include "Consola.h"
#include "conexionMemoria.h"
#include <../SharedLibrary/loggers.h>

const char* path_configs ="/home/utnso/workspace/tp-2019-1c-Macacos/Kernel/kernel.properties";
#define KERNEL_LOG_PATH "./kernel.log"



#define PROGRAM_NAME "KERNEL"

#define ACTIVE_CONSOLE 1
void crearPrimerMemoria();
memoria *crearMemoria(int,char*);
void initConfiguracion();
void initThreadAPI ();
void setearValores(t_config * archivoConfig);
void interPlanificador();



pthread_t threadConexionMemoria;
pthread_t threadConsola;
pthread_t threadPlanificador;
pthread_t	threadInterPlanificador;


#endif /* KERNEL_H_ */
