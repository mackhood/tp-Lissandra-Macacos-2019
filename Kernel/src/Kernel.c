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

void setearValores(t_config * archivoConfig) {}



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
	pthread_create(&threadNotifier, NULL, (void*)notifier, NULL);
	pthread_create(&threadPlanificador, NULL, (void*)pasarArunnign, NULL);

	pthread_detach(threadPlanificador);
	pthread_detach(threadNotifier);
	pthread_detach(threadConsola );
	pthread_detach(threadInterPlanificador);
	pthread_detach(threadConexionMemoria);
	pthread_detach(threadEstadisticas);
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


	while(!destProtocol)
	{





		int i;
		int tiempoReadsTotalEC = 0;
		int tiempoReadsTotalSC = 0;
		int tiempoReadsTotalSHC = 0;
		int tiempoWritesTotalEC = 0;
		int tiempoWritesTotalSC = 0;
		int tiempoWritesTotalSHC = 0;
		int writesTotalEC = 0;
		int writesTotalSC = 0;
		int writesTotalSHC = 0;
		int readsTotalEC = 0;
		int readsTotalSC = 0;
		int readsTotalSHC = 0;
		for( i=0 ; i< list_size(tKernel->memoriasConCriterio);i++)
		{
			memoria* unaMemoria =	list_get(tKernel->memoriasConCriterio,i);
			tiempoReadsTotalSC += unaMemoria->estadisticasMemoriaSC->Read_Latency;
			readsTotalSC += unaMemoria->estadisticasMemoriaSC->Reads;
			tiempoWritesTotalSC += unaMemoria->estadisticasMemoriaSC->Write_Latency;
			writesTotalSC += unaMemoria->estadisticasMemoriaSC->Writes;
			tiempoReadsTotalSHC += unaMemoria->estadisticasMemoriaSHC->Read_Latency;
			readsTotalSHC += unaMemoria->estadisticasMemoriaSHC->Reads;
			tiempoWritesTotalSHC += unaMemoria->estadisticasMemoriaSHC->Write_Latency;
			writesTotalSHC += unaMemoria->estadisticasMemoriaSHC->Writes;
			tiempoReadsTotalEC += unaMemoria->estadisticasMemoriaEC->Read_Latency;
			readsTotalEC += unaMemoria->estadisticasMemoriaEC->Reads;
			tiempoWritesTotalEC += unaMemoria->estadisticasMemoriaEC->Write_Latency;
			writesTotalEC += unaMemoria->estadisticasMemoriaEC->Writes;
		}
		logInfo("Estadísticas de las memorias del criterio SC:");
		logInfo("La cantidad de lecturas de SC fue: %s", string_itoa(readsTotalSC));
		if(readsTotalSC > 0)
		{
			logInfo("El tiempo promedio de cada lectura de SC fue: %s", string_itoa(tiempoReadsTotalSC/readsTotalSC));
		}
		else
		{
			logInfo("No hubo lecturas de SC, por ende no hay tiempo que calcular");
		}
		logInfo("La cantidad de escrituras de SC fue: %s",string_itoa(writesTotalSC));
		if(writesTotalSC > 0)
		{
			logInfo("El tiempo promedio de cada escritura de SC fue: %s", string_itoa(tiempoReadsTotalSC/writesTotalSC));
		}
		else
		{
			logInfo("No hubo escrituras de SC, por ende no hay tiempo que calcular");
		}
		logInfo("Estadísticas de las memorias del criterio SHC:");
		logInfo("La cantidad de lecturas de SHC fue: %s", string_itoa(readsTotalSHC));
		if(readsTotalSHC > 0)
		{
			logInfo("El tiempo promedio de cada lectura de SHC fue: %s", string_itoa(tiempoReadsTotalSHC/readsTotalSHC));
		}
		else
		{
			logInfo("No hubo lecturas de SHC, por ende no hay tiempo que calcular");
		}
		logInfo("La cantidad de escrituras de SHC fue: %s",string_itoa(writesTotalSHC));
		if(writesTotalSHC > 0)
		{
			logInfo("El tiempo promedio de cada escritura de SHC fue: %s", string_itoa(tiempoWritesTotalSHC/writesTotalSHC));
		}
		else
		{
			logInfo("No hubo escrituras de SHC, por ende no hay tiempo que calcular");
		}
		logInfo("Estadísticas de las memorias del criterio EC:");
		logInfo("La cantidad de lecturas de EC fue: %s", string_itoa(readsTotalEC));
		if(readsTotalEC > 0)
		{
			logInfo("El tiempo promedio de cada lectura de EC fue: %s", string_itoa(tiempoReadsTotalEC/readsTotalEC));
		}
		else
		{
			logInfo("No hubo lecturas de EC, por ende no hay tiempo que calcular");
		}
		logInfo("La cantidad de escrituras de EC fue: %s",string_itoa(writesTotalEC));
		if(writesTotalEC > 0)
		{
			logInfo("El tiempo promedio de cada escritura de EC fue: %s", string_itoa(tiempoWritesTotalEC/writesTotalEC));
		}
		else
		{
			logInfo("No hubo escrituras de EC, por ende no hay tiempo que calcular");
		}
		logInfo("A continuacion, el Memory Load de cada una de las memorias que se encuentran conectadas");
		int r;
		for( r = 0 ; r< list_size(tKernel->memoriasConCriterio);r++)
		{
			memoria* unaMemoria =	list_get(tKernel->memoriasConCriterio,r);
			int totalRequestsRespondidas = unaMemoria->estadisticasMemoriaEC->Reads + unaMemoria->estadisticasMemoriaEC->Writes + unaMemoria->estadisticasMemoriaSC->Reads + unaMemoria->estadisticasMemoriaSC->Writes + unaMemoria->estadisticasMemoriaSHC->Reads + unaMemoria->estadisticasMemoriaSHC->Writes;
			if(totalRequestsRespondidas > 0)
			{
				double porcentajeRespondido = (double)(totalRequestsRespondidas)/(double)(t_estadisticas->Reads + t_estadisticas->Writes);
				logInfo("La memoria %d se encargó del %lf (porciento) de las requests\n", unaMemoria->numeroMemoria,porcentajeRespondido*100);
			}
			else
			{
				logInfo("La memoria %d no recibió ninguna request", unaMemoria->numeroMemoria);
			}
			reestablecerEstadisticasMemoria(unaMemoria);
		}







	}


	reestablecerEstadisticas();


	sleep(30);




}







void crearPrimerMemoria(){


	memoria* memoriaNueva = malloc(sizeof(memoria));

	int conexion  = conectar_a_memoria_flexible(tKernel->config->ip_memoria,tKernel->config->puerto_memoria,KERNEL);
	prot_enviar_mensaje(conexion,HANDSHAKE,0,NULL);
	t_prot_mensaje * recepcion = prot_recibir_mensaje(conexion);

	int otroNumero = *(int*)(recepcion->payload);
	close(conexion);

	memoriaNueva = (memoria*)crearMemoria(tKernel->config->ip_memoria, tKernel->config->puerto_memoria,otroNumero);



	t_Criterios->strongConsistency = memoriaNueva;


	configuracion = memoriaNueva;
	list_add(tKernel->memoriasConCriterio,memoriaNueva);
	queue_push(tKernel->memoriasCola,memoriaNueva);


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




