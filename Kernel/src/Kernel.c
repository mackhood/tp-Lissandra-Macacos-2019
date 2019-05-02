/*
 ============================================================================
 Name        : Kernel.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "Kernel.h"







void setearValores(t_config * archivoConfig) {


}



void initConfiguracion(){



	t_config* config = config_create("/home/utnso/workspace/tp-2019-1c-Macacos/Kernel/kernel.properties");
	if(config  == NULL){
		perror("Error al abrir el archivo de configuracion");
		free(config);
		exit(1);
	}

	/**--------------- cargo todos los datos de kernel.configs -------------------------**/
	kernel_config* kernelConfig= malloc(sizeof(kernel_config));

	kernelConfig->puerto_memoria =config_get_int_value(config,"PUERTO_MEMORIA");
	kernelConfig->quantum = config_get_int_value(config,"QUANTUM");
	kernelConfig->metadata_refresh = config_get_int_value(config,"METADATA_REFRESH");
	kernelConfig->sleep_ejecucion = config_get_int_value(config,"SLEEP_EJECUCION");
	kernelConfig->multiprocesamiento= config_get_int_value(config,"MULTIPROCESAMIENTO");
	char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
	kernelConfig->ip_memoria = malloc( string_length(ip_memoria) + 1 );
	strcpy(kernelConfig->ip_memoria, ip_memoria);
	config_destroy(config);


	//logueo todos los datos de configuracion
	initMutexlog(KERNEL_LOG_PATH,PROGRAM_NAME,ACTIVE_CONSOLE,LOG_LEVEL_TRACE);

	logInfo("IP de memoria :%s -Metadata refresh: %d -Multiprocesamiento: %d -Puerto Memoria: %d -Quantum: %d  -Sleep ejecucion: %d\n",
				kernelConfig->ip_memoria,
				kernelConfig->metadata_refresh,
				kernelConfig->multiprocesamiento,
				kernelConfig->puerto_memoria,
				kernelConfig->quantum,
				kernelConfig->sleep_ejecucion
	);

	tKernel = malloc(sizeof(t_kernel));
	tKernel->config = kernelConfig;
	tKernel->memoriasSincriterio = list_create();
	initConfigAdminColas();

}



void initThread(){

	logInfo("Creando thread para atender las conexiones de memoria");
	pthread_create(&threadConexionMemoria, NULL, (void*) handler_conexion_memoria, tKernel);
	logInfo("Creando thread para el funcionamiento de la consola");
	pthread_create(&threadConsola, NULL, (void*)handleConsola, tKernel);



	//pthread_create(&threadPlanificador, NULL, (void*)handlePlanificadorLP, tKernel);


	pthread_join(threadConsola ,NULL);
	pthread_join(threadPlanificador ,NULL);
	pthread_join(threadConexionMemoria, NULL);

}






