#include "Lissandra.h"

void inicializar()
{
	hilos = list_create();
	memorias = list_create();
	compactadores = list_create();
	iniciarServidor();
}

void mainLissandra ()
{
	inicializar();
}

void setearValores(t_config * archivoConfig)
{
	retardo = config_get_int_value(archivoConfig, "RETARDO");
	server_puerto = config_get_int_value(archivoConfig, "PUERTO_ESCUCHA");
}

void iniciarServidor()
{
	socket_memoria = levantar_servidor(server_puerto);
	
}
