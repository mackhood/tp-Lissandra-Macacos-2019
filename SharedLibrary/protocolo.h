/*
 * protocolo.h
 *
 *  Created on: 2 abr. 2019
 *      Author: utnso
 */

#ifndef PROTOCOLO_H_
#define PROTOCOLO_H_

//Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define VALOR_EXIT 1
#define FALLO_ENVIAR_MENSAJE "No se pudo enviar mensaje a traves del socket"
#define LARGO_MAXIMO_CLAVE 40

//HEADER
typedef enum{




	CONEXION, //Un cliente informa a un servidor que se ha conectado. Payload: Algun t_cliente
	FALLO_AL_RECIBIR,//Indica que un mensaje no se recibio correctamente en prot_recibir_mensaje
	DESCONEXION, //Indica que un cliente se ha desconectado (usado para instancias)
	FALLO_AL_ENVIAR,//Todavia no aplicado. Analogo al de arriba

	//Kernel-Memoria
	HANDSHAKE_KERNEL_MEMORIA,//CUANDO SE REALIZA LA PRIMERA CONEXION LA MEMORIA Y EL KERNEL

	KEY_SOLICITADA_SELECT,
	//Memoria-FileSystem
	HANDSHAKE,
	SOLICITUD_TABLA,
	VALUE_SOLICITADO_OK,
	INSERTAR_KEY,
	CREATE_TABLA,
	TABLA_CREADA_OK,
	TABLA_CREADA_YA_EXISTENTE,
	TABLA_CREADA_FALLO,
	TABLE_DROP,
	TABLE_DROP_OK,
	TABLE_DROP_FALLO,
	TABLE_DROP_NO_EXISTE,
	DESCRIBE,
	FULL_DESCRIBE,
	POINT_DESCRIBE,
	FAILED_DESCRIBE,
	JOURNALING_INSERT,
	INSERT_SUCCESSFUL,
	INSERT_FAILED_ON_MEMTABLE,
	INSERT_FAILURE,POOL_MEMORIA


}t_header;

typedef struct{
	t_header head;
	size_t tamanio_total;
	void* payload;
}t_prot_mensaje;

//Para t_clientes
//typedef enum{
//	//aca iria el tipo de cliente dependiendo de si necesitamos distinguir que socket cliente llega
//}t_cliente;

/**
* @NAME: prot_recibir_mensaje
* @DESC: retorna un mensaje recibido en el socket.
* 		 El mensaje tiene la siguiente forma: HEADER + PAYLOAD
* 		 Quien recibe el mensaje debe castearlo asi:
* 		 t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(un_socket);
* 		 t_header header_recibido = mensaje_recibido->head;
* 		 t_algo payload = *(t_algo*) mensaje_recibido->payload;
*
* @PARAMS:
* 		socket_origen - el nombre lo dice...
*/
t_prot_mensaje* prot_recibir_mensaje(int socket_origen);

/**
* @NAME: mensaje_error_al_recibir
* @DESC: retorno de un prot_recibir_mensaje si hay un error al recibir
*/
t_prot_mensaje* mensaje_error_al_recibir();

/**
* @NAME: mensaje_desconexion_al_recibir
* @DESC: retorno de un prot_recibir_mensaje si hay desconexion
*/
t_prot_mensaje* mensaje_desconexion_al_recibir();

/**
* @NAME: prot_enviar_mensaje
* @DESC: funcion SUPER GENERICA que resume todas las anteriores
* 		 Basicamente, le pones el socket destino, el HEADER que queres mandar, el sizeof del payload, el puntero al payload (osea, lo pasas con &) y te hace todo el trabajo solo
*		 Ejemplo: quiero mandar un int, con el header NUMERO -supongamos que existe
*		 int a = 8
*		 prot_enviar_mensaje( destino, NUMERO, sizeof(int), &a);
*		 LISTOOOO
* @PARAMS
* 		socket_destino - el nombre lo dice...
* 		header - el header
* 		tamanio_payload - el sizeof(tipo_de_lo_que_quiero_enviar)
* 		payload - &cosa_que_quiero_enviar
*/
void prot_enviar_mensaje(int socket_destino,t_header header , size_t tamanio_payload, void* payload);

//Hace los free
void prot_destruir_mensaje(t_prot_mensaje* victima);

/*Bien, la idea de esto es que cuando se haga un set sobre una clave
 * se deberia tratar a la misma como una sola estructura para que su manejo sea mas
 * sencillao: solo se enviaria un mensaje que tiene las cadenas (clave y valor) y sus
 * tamaños para hacer el parseo mas sencillo
 *
 * Buffer: tamanio_clave + tamanio_valor + clave + valor
 * */
void* prot_armar_payload_con_clave_valor(char* clave, char* valor);

/*Para cuando el payload es un string simple.
 * Lo lee y retorna el char* al mensaje
 * Hace el malloc dentro*/
char* leer_string_de_mensaje(t_prot_mensaje* mensaje);

#endif /* PROTOCOLO_H_ */
