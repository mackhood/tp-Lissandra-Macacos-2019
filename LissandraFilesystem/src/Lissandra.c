#include "Lissandra.h"

void inicializar()
{
	hilos = list_create();
	memorias = list_create();
	compactadores = list_create();

	//iniciarServidor();
}

void setearValoresLissandra(t_config * archivoConfig)
{
	retardo = config_get_int_value(archivoConfig, "RETARDO");
	server_puerto = config_get_int_value(archivoConfig, "PUERTO_ESCUCHA");
	server_ip = strdup(config_get_string_value(archivoConfig,"IP_FILE_SYSTEM"));
}

void mainLissandra ()
{
	inicializar();
}

void iniciarServidor()
{
	socket_memoria = levantar_servidor(server_puerto);
}

void insertKeysetter(char* tabla, uint16_t key, char* value, time_t timestamp)
{
/*	struct t_memtablekey* key = malloc(sizeof(t_memtablekey));
	key->keysetter = clave;
	strcpy(key->tabla, tabla);*/
}

t_keysetter selectKey(char* tabla, uint16_t receivedKey)
{
	t_keysetter key;
	return key;
}

int llamadoACrearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	switch (crearTabla(nombre, consistencia, particiones, tiempoCompactacion))
	{
		case 0:
		{
			return 0;
			break;
		}
		case 1:
		{
			return 1;
			break;
		}
		case 2:
		{
			return 2;
			break;
		}
		default:
			return 1;
		break;
	}
}
