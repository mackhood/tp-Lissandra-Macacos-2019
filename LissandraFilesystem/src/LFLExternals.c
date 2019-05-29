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
	fclose(FP);
	return tamanio;
}

t_list* parsearKeys(t_list* clavesAParsear)
{
	t_list* clavesPostParseo = list_create();
	int parserListPointer = 0;
	char* keyHandler;
	while((keyHandler = list_get(clavesAParsear, parserListPointer)) != NULL)
	{
		t_keysetter* helpingHand = malloc(sizeof(t_keysetter) + 3);
		int parserPointer = 0;
		int handlerSize = strlen(keyHandler);
		char* key = malloc(24);
		char* value = malloc(tamanio_value + 1);
		char* timestamp = malloc(14);
		int status = 0;
		int k = 1;
		int v = 1;
		int t = 1;
		while(parserPointer < handlerSize)
		{
			switch(keyHandler[parserPointer])
			{
			case ';':
			{
				parserPointer++;
				status++;
				break;
			}
			case '\n':
			{
				helpingHand = construirKeysetter(timestamp, key, value);
				list_add(clavesPostParseo, helpingHand);
				parserPointer++;
				status = 0;
				k = 1;
				v = 1;
				t = 1;
				break;
			}
			default:
			{
				char* aux = malloc(2);
				aux[0] = keyHandler[parserPointer];
				aux[1] = '\0';
				switch(status)
				{
				case 0:
				{
					if(t)
					{
						strcpy(timestamp, aux);
						t = 0;
					}
					else
						strcat(timestamp, aux);
					break;
				}
				case 1:
				{
					if(k)
					{
						strcpy(key, aux);
						k = 0;
					}
					else
						strcat(key, aux);
					break;
				}
				case 2:
				{
					if(v)
					{
						strcpy(value, aux);
						v = 0;
					}
					else
						strcat(value, aux);
					break;
				}
				}
				free(aux);
				parserPointer++;
				break;
			}
			}
		}
		parserListPointer++;
	}
	free(keyHandler);
	return clavesPostParseo;
}

t_list* inversaParsearKeys(t_list* clavesADesparsear)
{
	t_list* clavesParseadas = list_create();
	int a = 0;
	while(NULL != list_get(clavesADesparsear, a))
	{
		t_keysetter* keyAuxiliar = list_get(clavesADesparsear, a);
		char* keyConvertida = malloc(strlen(string_itoa(keyAuxiliar->key))
				+ strlen(string_from_format("%lf", keyAuxiliar->timestamp)) + strlen(keyAuxiliar->clave) + 3);
		strcpy(keyConvertida, string_itoa(keyAuxiliar->key));
		strcat(keyConvertida, ";");
		char* aux = string_from_format("%lf", keyAuxiliar->timestamp);
		strcat(keyConvertida, string_substring_until(aux, strlen(aux) - 7));
		strcat(keyConvertida, ";");
		strcat(keyConvertida, keyAuxiliar->clave);
		strcat(keyConvertida, "\n");
		list_add(clavesParseadas, keyConvertida);
		a++;
	}
	return clavesParseadas;
}
