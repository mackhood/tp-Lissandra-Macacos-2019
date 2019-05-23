/*
 * LFLExternals.c
 *
 *  Created on: 6 may. 2019
 *      Author: utnso
 */

#include "LFLExternals.h"

int chequearTimestamps(t_Memtablekeys* key1, t_Memtablekeys* key2)
{
	return (key1->data->timestamp > key2->data->timestamp);
}

int obtenerTamanioArchivoConfig(char* direccionArchivo)
{
	t_config* archivo = config_create(direccionArchivo);
	int tamanio = config_get_int_value(archivo, "SIZE");
	config_destroy(archivo);
	return tamanio;
}

