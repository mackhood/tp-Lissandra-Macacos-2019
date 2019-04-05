#include "LissandraFileSistem.h"

int main(void)
{
	char* lissandraFL_log_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/LissandraFileSystem.log");
	loggerLFL = crearLogger(lissandraFL_log_ruta, "LissandraFileSystem");
	char* lissandraFL_config_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFilesystem/src/Lissandra.cfg");
	leerConfig(lissandraFL_config_ruta,loggerLFL);
	hilos = list_create();
	iniciarConsola();


	return EXIT_SUCCESS;
}

void iniciarConsola(){
	pthread_t hilo;
	log_info(loggerLFL,"MAIN: Se inicio un hilo para manejar la consola.");
	pthread_create(&hilo, NULL, (void *) consola, NULL);
	pthread_detach(hilo);
}
