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


	metadata* m_etadata = malloc(sizeof(metadata));
	m_etadata->tablas = list_create();

	tMetadata = m_etadata;

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
	t_list* criterioss = list_create();
	tKernel = malloc(sizeof(t_kernel));
	tKernel->config = kernelConfig;
	tKernel->memoriasSincriterio = criterioss;
	initConfigAdminColas();
	crearPrimerMemoria();
}



void initThread(){

	logInfo("Creando thread para atender las conexiones de memoria");
	pthread_create(&threadConexionMemoria, NULL, (void*) handler_conexion_memoria, tKernel);
	logInfo("Creando thread para el funcionamiento de la consola");
	pthread_create(&threadConsola, NULL, (void*)handleConsola,NULL);
	pthread_create(&threadInterPlanificador,NULL,(void*)interPlanificador,NULL);


	pthread_create(&threadPlanificador, NULL, (void*)pasarArunnign(),NULL);
	pthread_detach(threadPlanificador );

	pthread_detach(threadConsola );
	pthread_detach(threadInterPlanificador);
	pthread_detach(threadConexionMemoria);

}


void interPlanificador(){




	DTB_KERNEL* dtb=(DTB_KERNEL*)getDTBNew();
				enviarAReady(dtb);

}





void crearPrimerMemoria(){


	memoria* memoriaNueva = malloc(sizeof(memoria));

	memoriaNueva = (memoria*)crearMemoria(tKernel->config->puerto_memoria,tKernel->config->ip_memoria);

	t_Criterios = malloc(sizeof(criterios));
	t_Criterios->strongConsistency = (memoria*)crearMemoria(tKernel->config->puerto_memoria,tKernel->config->ip_memoria);

	configuracion = memoriaNueva;



}



int actualIdMemoria = 0 ;

	int getIdMemoria(){
			int id;
			pthread_mutex_lock(&mutexIdMemoria);
			id = actualIdMemoria++;
			pthread_mutex_unlock(&mutexIdMemoria);
			return id;
		}


memoria* crearMemoria(int puerto,char* ip){

	memoria* nuevaMemoria = malloc(sizeof(memoria));
	nuevaMemoria->puerto = puerto;
	nuevaMemoria->numeroMemoria = getIdMemoria();
	nuevaMemoria->estaEjecutando =0;
	nuevaMemoria->ip = ip;

	return nuevaMemoria;
}





