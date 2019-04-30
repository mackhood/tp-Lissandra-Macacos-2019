/*
 * conexiones.h
 *
 *  Created on: 1 abr. 2019
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <readline/readline.h>
#include <commons/log.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include "protocolo.h"
#include "auxiliaryFunctions.h"

//Mensajes de error
#define ERROR_CREAR_SOCKET "Crear socket con socket()"
#define ERROR_RESEVAR_PUERTO "No se pudo reservar puerto con bind()"
#define ERROR_ESCUCHAR_PUERTO "No se pudo escuchar puerto con listen()"
#define ERROR_ACEPTAR_CLIENTE "No se pudo aceptar cliente con accept()"
#define ERROR_CONECTAR_SERVIDOR "No se pudo conectar al servidor"
#define Error_enviar_mensaje "No se pudo enviar mensaje a traves del socket"
#define Fallo_recibir_mensaje "No se pudo recibir mensaje a traves del socket"

//Valores hardcodeados
#define TIEMPO_SLEEP 2
#define VALOR_EXIT 1 //lo pongo aca porque no entiendo que hace y si hace algo y despues hay que cambiar es mas facil
#define MAX_CLIENTES_ENCOLADOS 128 //no buscar el porque son 128

/*Prototipos*/
int levantar_servidor(int puerto);
void crear_socket(int *socket);
int conectar_a_servidor(char* ip, int puerto, char* nombreCliente);
void print_error(char *mensaje);


#endif /* CONEXIONES_H_ */
