#include "LissandraFileSistem.h"

int main(void)
{
	char* lissandraFL_log_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/LissandraFileSystem.log");
	loggerLFL = crearLogger(lissandraFL_log_ruta, "LissandraFileSystem");
	char* lissandraFL_config_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/src/Lissandra.cfg");
	leerConfig(lissandraFL_config_ruta,loggerLFL);
	hilosLFL = list_create();
	iniciar();
	return EXIT_SUCCESS;
}

void setearValores(t_config * archivoConfig)
{
	setearValoresLissandra(archivoConfig);
	setearValoresFileSistem(archivoConfig);
}

void iniciar()
{
	iniciarConsola();
	iniciarLissandra();
	iniciarCompactador();
	iniciarFileSistem();
}
void iniciarConsola(){
	pthread_t hiloConsola;
	log_info(loggerLFL,"MAIN: Se inicio un hilo para manejar la consola.");
	pthread_create(&hiloConsola, NULL, (void *) consola, NULL);
	pthread_join(hiloConsola, NULL);
}

void iniciarLissandra()
{
	pthread_t hiloLissandra;
	log_info(loggerLFL,"MAIN: Se inicio un hilo para manejar a Lissandra.");
	pthread_create(&hiloLissandra, NULL, (void *) mainLissandra, NULL);
	pthread_detach(hiloLissandra);
}

void iniciarCompactador()
{
	pthread_t hiloCompactador;
	log_info(loggerLFL,"MAIN: Se inicio un hilo para manejar el Compactador.");
	pthread_create(&hiloCompactador, NULL, (void *) mainCompactador, NULL);
	pthread_detach(hiloCompactador);
}

void iniciarFileSistem()
{
	pthread_t hiloFileSistem;
	log_info(loggerLFL,"MAIN: Se inicio un hilo para manejar el FileSistem.");
	pthread_create(&hiloFileSistem, NULL, (void *) mainFileSistem, NULL);
	pthread_detach(hiloFileSistem);
}