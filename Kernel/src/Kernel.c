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

#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (1024* (EVENT_SIZE + 16))





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
	initMutexlog(KERNEL_LOG_PATH,PROGRAM_NAME,0,LOG_LEVEL_TRACE);

	logInfo("IP de memoria :%s -Metadata refresh: %d -Multiprocesamiento: %d -Puerto Memoria: %d -Quantum: %d  -Sleep ejecucion: %d\n",
			kernelConfig->ip_memoria,
			kernelConfig->metadata_refresh,
			kernelConfig->multiprocesamiento,
			kernelConfig->puerto_memoria,
			kernelConfig->quantum,
			kernelConfig->sleep_ejecucion
	);

	t_estadisticas = malloc(sizeof(estadisticas));
	tKernel = malloc(sizeof(t_kernel));
	tKernel->config = kernelConfig;
	tKernel->memoriasSinCriterio = list_create();
	tKernel->memoriasConCriterio = list_create();
	tKernel->memoriasCola = queue_create();
	tKernel->memorias=list_create();

	t_Criterios = malloc(sizeof(criterios));
	t_Criterios->StrongHash = list_create();
	t_Criterios->eventualConsistency = queue_create();

	tKernel->primerConexion=1;
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
	pthread_create(&threadNotifier, NULL, (void*)notifier, NULL)
	pthread_create(&threadPlanificador, NULL, (void*)pasarArunnign(),NULL);

	pthread_detach(threadPlanificador);
	pthread_detach(threadNotifier);
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
	t_estadisticas->Read_Latency = 0;
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

	memoriaNueva = (memoria*)crearMemoria(tKernel->config->ip_memoria, tKernel->config->puerto_memoria);



	t_Criterios->strongConsistency = memoriaNueva;


	configuracion = memoriaNueva;
	list_add(tKernel->memoriasConCriterio,memoriaNueva);
	queue_push(tKernel->memoriasCola,memoriaNueva);


}



int actualIdMemoria = 0 ;

int getIdMemoria(){
	int id = 0;
	pthread_mutex_lock(&mutexIdMemoria);
	actualIdMemoria++;
	id = actualIdMemoria;
	pthread_mutex_unlock(&mutexIdMemoria);
	return id;
}


memoria* crearMemoria(char* ip,int puerto){

	memoria* nuevaMemoria = malloc(sizeof(memoria));
	nuevaMemoria->puerto = puerto;
	nuevaMemoria->numeroMemoria = getIdMemoria();
	nuevaMemoria->estaEjecutando =0;
	nuevaMemoria->ip = ip;
	estadisticas * estructuraSC = malloc(sizeof(estadisticas));
	estructuraSC->Read_Latency = 0;
	estructuraSC->Reads = 0;
	estructuraSC->Write_Latency = 0;
	estructuraSC->Writes = 0;
	estadisticas * estructuraSHC = malloc(sizeof(estadisticas));
	estructuraSHC->Read_Latency = 0;
	estructuraSHC->Reads = 0;
	estructuraSHC->Write_Latency = 0;
	estructuraSHC->Writes = 0;
	estadisticas * estructuraEC = malloc(sizeof(estadisticas));
	estructuraEC->Read_Latency = 0;
	estructuraEC->Reads = 0;
	estructuraEC->Write_Latency = 0;
	estructuraEC->Writes = 0;
	nuevaMemoria->estadisticasMemoriaSC = estructuraSC;
	nuevaMemoria->estadisticasMemoriaSHC = estructuraSHC;
	nuevaMemoria->estadisticasMemoriaEC = estructuraEC;
	nuevaMemoria->insertsTotales=0;
	nuevaMemoria->selectTotales=0;
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

void notifier()
{
	int length;
	int i = 0;
	char buffer[BUF_LEN];

	int fileToWatch = inotify_init();
	if( fileToWatch < 0)
	{
		logError("[Kernel]: Error al iniciar el notifier.");
		return;
	}

	int watchDescriptor = inotify_add_watch(fileToWatch, path_configs, IN_CREATE | IN_MODIFY | IN_DELETE | IN_DELETE_SELF);
	length = read(fileToWatch, buffer, BUF_LEN);

	if(length < 0)
	{
		logError("[Kernel]: Error al tratar de acceder al archivo a vigilar.");
		return;
	}

	while(i < length)
	{
		struct inotify_event* event = (struct inotify_event*) &buffer[i];
		if(event->mask & IN_MODIFY)
		{
			sleep(2);
			logInfo("[Lissandra]: se ha modificado el archivo de configuración, actualizando valores.");
			t_config* ConfigMain = config_create(path_configs);
			tKernel->config->quantum = config_get_int_value(ConfigMain,"QUANTUM");
			tKernel->config->metadata_refresh = config_get_int_value(ConfigMain,"METADATA_REFRESH");
			tKernel->config->sleep_ejecucion = config_get_int_value(ConfigMain,"SLEEP_EJECUCION");
			config_destroy(ConfigMain);
			logInfo("[Kernel]: valores actualizados.");

		}
		else if(event->mask & IN_DELETE_SELF)
		{
			logInfo("[Kernel]: Se ha detectado que el archivo de config ha sido eliminado. Terminando sistema.");
			puts("El Kernel está siendo desconectado ya que el archivo de configuración fue destruido.");
			break;
		}
		else if(event->mask & IN_CREATE)
		{
			logInfo("[Kernel]: Se ha detectado el archivo de configuración");
		}
		length = read(fileToWatch, buffer, BUF_LEN);
	}
	(void) inotify_rm_watch(fileToWatch, watchDescriptor);
	(void) close(fileToWatch);
}




