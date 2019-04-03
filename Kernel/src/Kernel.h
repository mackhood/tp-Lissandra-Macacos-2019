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

const char* path_configs ="/home/utnso/workspace/tp-2019-1c-Macacos/Kernel/kernel.properties";
#define KERNEL_LOG_PATH "./kernel.log"



#define PROGRAM_NAME "KERNEL"

#define ACTIVE_CONSOLE 1

void initConfiguracion();
void initThreadAPI ();






#endif /* KERNEL_H_ */