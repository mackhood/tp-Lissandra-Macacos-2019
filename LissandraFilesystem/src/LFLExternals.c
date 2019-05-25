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

int chequearTimeKey(t_keysetter* key1, t_keysetter* key2)
{
	return (key1->timestamp > key2->timestamp);
}

int obtenerTamanioArchivoConfig(char* direccionArchivo)
{
	t_config* archivo = config_create(direccionArchivo);
	int tamanio = config_get_int_value(archivo, "SIZE");
	config_destroy(archivo);
	return tamanio;
}

t_keysetter* construirKeysetter(char* timestamp, char* key, char* value)
{
	t_keysetter* theReturn = malloc(sizeof(t_keysetter) + 3);
	theReturn->timestamp = (double)atoll(timestamp);
	theReturn->key = atoi(key);
	theReturn->clave = value;
	return theReturn;
}

unsigned long obtenerTamanioArchivo(char* direccionArchivo)
{
	FILE* FP = fopen(direccionArchivo, "r+");
	fseek(FP, 0, SEEK_END);
	unsigned long tamanio = (unsigned long)ftell(FP);
	return tamanio;
}

