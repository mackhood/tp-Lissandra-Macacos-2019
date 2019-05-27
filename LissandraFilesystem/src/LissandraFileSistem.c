#include "LissandraFileSistem.h"

int main(void)
{
	char* lissandraFL_log_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/LissandraFileSystem.log");
	logger = crearLogger(lissandraFL_log_ruta, "LissandraFileSystem");
	logInfo("Main: Atra estherni ono thelduin, ai fricai.");
	char* lissandraFL_config_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/src/Lissandra.cfg");
	leerConfig(lissandraFL_config_ruta,logger);
	hilosLFL = list_create();
	iniciar();
	for(;;)
	{
		if(signalExit == true)
		{
			terminationProtocol();
			break;
		}
	}
	free(lissandraFL_log_ruta);
	free(lissandraFL_config_ruta);
	return EXIT_SUCCESS;
}

void setearValores(t_config * archivoConfig)
{
	setearValoresLissandra(archivoConfig);
	setearValoresFileSistem(archivoConfig);
	setearValoresCompactador(archivoConfig);
}

void iniciar()
{
	selectActivo = 0;
	pthread_mutex_init(&compactacionActiva, NULL);
	pthread_mutex_init(&dumpEnCurso, NULL);
	iniciarLissandra();
	iniciarCompactador();
	iniciarFileSistem();
	iniciarConsola();
}

void iniciarConsola(){
	pthread_t hiloConsola;
	signalExit = false;
	logInfo("MAIN: Se inicio un hilo para manejar la consola.");
	pthread_create(&hiloConsola, NULL, (void *) consola, NULL);
	pthread_detach(hiloConsola);
}

void iniciarLissandra()
{
	pthread_t hiloLissandra;
	logInfo("MAIN: Se inicio un hilo para manejar a Lissandra.");
	pthread_create(&hiloLissandra, NULL, (void *) mainLissandra, NULL);
	pthread_detach(hiloLissandra);
}

void iniciarCompactador()
{
	pthread_t hiloCompactador;
	logInfo("MAIN: Se inicio un hilo para manejar el Compactador.");
	pthread_create(&hiloCompactador, NULL, (void *) mainCompactador, NULL);
	pthread_detach(hiloCompactador);
}

void iniciarFileSistem()
{
	pthread_t hiloFileSistem;
	logInfo("MAIN: Se inicio un hilo para manejar el FileSistem.");
	pthread_create(&hiloFileSistem, NULL, (void *) mainFileSistem, NULL);
	pthread_detach(hiloFileSistem);
}

void terminationProtocol()
{
	logInfo("Main: El termination protocol ha sido inicializado, apagando sistema.");
	list_destroy(hilosLFL);
	killProtocolLissandra();
	killProtocolCompactador();
	killProtocolFileSystem();
	free(punto_montaje);
	list_destroy(memtable);
	list_destroy(tablasEnEjecucion);
	logInfo("Main: Waise neiat.");
	log_destroy(logger);
}
