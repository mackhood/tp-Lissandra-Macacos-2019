#include "Lissandra.h"

void inicializar()
{
	hilos = list_create();
	memorias = list_create();
	compactadores = list_create();

	iniciarServidor();
}
void setearValoresLissandra(t_config * archivoConfig)
{
	retardo = config_get_int_value(archivoConfig, "RETARDO");
	server_puerto = config_get_int_value(archivoConfig, "PUERTO_ESCUCHA");
}
void mainLissandra ()
{
	inicializar();
}


void iniciarServidor()
{
	socket_memoria = levantar_servidor(server_puerto);
}

void insertKeysetter(t_keysetter * clave, char* tabla)
{
/*	struct t_memtablekey* key = malloc(sizeof(t_memtablekey));
	key->keysetter = clave;
	strcpy(key->tabla, tabla);*/
}
