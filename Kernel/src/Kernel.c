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

	srand((unsigned) time(&randomNumbertime));






	destProtocol = 0;
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
	t_list* memoriasGossiping = list_create();
	t_list* memorias = list_create();
	t_list* nuevasMemorias = list_create();
	t_estadisticas = malloc(sizeof(estadisticas));
	tKernel = malloc(sizeof(t_kernel));
	tKernel->config = kernelConfig;
	tKernel->memoriasSinCriterio = memoriasGossiping;
	tKernel->memoriasConCriterio = memorias;
	tKernel->memorias=nuevasMemorias;
	initConfigAdminColas();
	crearPrimerMemoria();
	reestablecerEstadisticas();

}



void initThread(){

	logInfo("Creando thread para atender las conexiones de memoria");
	pthread_create(&threadConexionMemoria, NULL, (void*) handler_conexion_memoria, tKernel);
	logInfo("Creando thread para el funcionamiento de la consola");
	pthread_create(&threadConsola, NULL, (void*)handleConsola,NULL);
	pthread_create(&threadInterPlanificador,NULL,(void*)interPlanificador,NULL);
	pthread_create(&threadEstadisticas, NULL, (void*)handleEstadisticas,NULL);

	pthread_create(&threadPlanificador, NULL, (void*)pasarArunnign(),NULL);
	pthread_detach(threadPlanificador );

	pthread_detach(threadConsola );
	pthread_detach(threadInterPlanificador);
	pthread_detach(threadConexionMemoria);

}


void interPlanificador(){


while(!destProtocol){

	DTB_KERNEL* dtb=(DTB_KERNEL*)getDTBNew();
				enviarAReady(dtb);

}

}



void reestablecerEstadisticas(){

	t_estadisticas->Reads = 0;
	t_estadisticas ->Read_Latency = 0;
	t_estadisticas->Write_Latency =0;
	t_estadisticas->Writes = 0;


}



void handleEstadisticas(){


	while(!destProtocol){

			logInfo("Estadisticas globales de todos los criterios");
			logInfo(string_itoa(t_estadisticas->Reads));
			logInfo(string_itoa(t_estadisticas ->Read_Latency));
			logInfo(string_itoa(t_estadisticas->Write_Latency));
			logInfo(string_itoa(t_estadisticas->Writes));
			int i;
			for( i=0 ; i< list_size(tKernel->memorias);i++){

			memoria* unaMemoria =	list_get(tKernel->memoriasConCriterio,i);
			logInfo("Memoria Numero : %d ", unaMemoria->numeroMemoria);

			logInfo("Criterio SC:");

			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Read_Latency));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Reads));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Write_Latency));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSC->Writes));

			if(unaMemoria->selectTotales == 0){


				logInfo("Memory Load para SC de esta memoria es 0");

			}else{


				logInfo("Memory Load para SC de esta memoria es : %f", unaMemoria->insertsTotales / unaMemoria->selectTotales);



			}


			logInfo("Criterio SHC:");



			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Read_Latency));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Reads));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Write_Latency));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaSHC->Writes));

			if(unaMemoria->selectTotales == 0){


				logInfo("Memory Load para SHC de esta memoria es 0");

			}else{


				logInfo("Memory Load para SHC de esta memoria es : %f", unaMemoria->insertsTotales / unaMemoria->selectTotales);



			}




			logInfo("Criterio EC:");





			logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Read_Latency));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Reads));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Write_Latency));
			logInfo(string_itoa(unaMemoria->estadisticasMemoriaEC->Writes));

			if(unaMemoria->selectTotales == 0){


				logInfo("Memory Load para EC de esta memoria es 0");

			}else{


				logInfo("Memory Load para EC de esta memoria es : %f", unaMemoria->insertsTotales / unaMemoria->selectTotales);



			}



			reestablecerEstadisticasMemoria(unaMemoria);


			}


			reestablecerEstadisticas();


			sleep(30);




	}






}




void crearPrimerMemoria(){


	memoria* memoriaNueva = malloc(sizeof(memoria));
	pthread_mutex_init(&memoriaNueva->enUso,NULL);

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
	estadisticas * estructura = malloc(sizeof(estadisticas));
	estructura->Read_Latency = 0;
	estructura->Reads = 0;
	estructura->Write_Latency = 0;
	estructura->Writes = 0;
	nuevaMemoria->estadisticasMemoriaSC = estructura;
	nuevaMemoria->estadisticasMemoriaSHC = estructura;
	nuevaMemoria->estadisticasMemoriaEC = estructura;
	nuevaMemoria->insertsTotales=0;
	nuevaMemoria->selectTotales=0;
	pthread_mutex_init(&nuevaMemoria,NULL);
	return nuevaMemoria;
}


void reestablecerEstadisticasMemoria(memoria * unaMemoria) {



unaMemoria->estadisticasMemoriaEC->Read_Latency=0;
unaMemoria->estadisticasMemoriaEC->Reads=0;
unaMemoria->estadisticasMemoriaEC->Write_Latency=0;
unaMemoria->estadisticasMemoriaEC->Writes=0;



unaMemoria->estadisticasMemoriaSC->Read_Latency=0;
unaMemoria->estadisticasMemoriaSC->Reads=0;
unaMemoria->estadisticasMemoriaSC->Write_Latency=0;
unaMemoria->estadisticasMemoriaSC->Writes=0;



unaMemoria->estadisticasMemoriaSHC->Read_Latency=0;
unaMemoria->estadisticasMemoriaSHC->Reads=0;
unaMemoria->estadisticasMemoriaSHC->Write_Latency=0;
unaMemoria->estadisticasMemoriaSHC->Writes=0;






}



