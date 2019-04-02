/*
 * kernelConfig.h
 *
 *  Created on: 2 abr. 2019
 *      Author: utnso
 */

#ifndef KERNELCONFIG_H_
#define KERNELCONFIG_H_





typedef struct{
	int	puerto_memoria;
	int	quantum;
	int multiprocesamiento;
	char* ip_memoria;
	int metadata_refresh;
	int sleep_ejecucion;
}kernel_config;

typedef struct {

	kernel_config* config;


}t_kernel;



t_kernel* tKernel;



#endif /* KERNELCONFIG_H_ */
