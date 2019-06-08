/*
 * conexionMemoria.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "conexionMemoria.h"

void handler_conexion_memoria(t_kernel* tKernel) {

while(1){


	int conexion=conectar_a_servidor(tKernel->config->ip_memoria,tKernel->config->puerto_memoria,"Memoria1");

	if (!conexion){

		logInfo("fallo conexion");

	}
	logInfo("Se conecto a una memoria de configuracion");




	prot_enviar_mensaje(conexion,DESCRIBE_REQ,0,NULL);

	t_prot_mensaje* mensaje_recibido1 = prot_recibir_mensaje(conexion);




	//PRIMERO DEBO PERDIR LA CANTIDAD DE MEMORIASCONECTADAS AL POOL DE MEMORIA

	t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(conexion);


	int tamanio ;
	memcpy(&tamanio,mensaje_recibido->payload,sizeof(int));

	char * mensaje = malloc(tamanio + 1);
	memcpy(mensaje,mensaje_recibido->payload + sizeof(int), tamanio);

	char ** tablaDescribe = string_split(mensaje, ";");


	int a=0;
	list_clean(tMetadata->tablas);
	while(tablaDescribe[a] != NULL  ){

		char* tabla = strtok(tablaDescribe[a],",");
		a++;

		bool  estaEnMetadata(void* nombre_tabla) {
			return  string_equals_ignore_case((char *) tabla,(char*)nombre_tabla);
		}


		if(!list_any_satisfy(tMetadata->tablas,(void*)estaEnMetadata))
		list_add(tMetadata->tablas,tabla);





	}

	close(conexion);







	//prot_enviar_mensaje(conexion,GOSSIPING,0,NULL);

	//t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(conexion);

	//mensaje_recibido





	sleep(tKernel->config->metadata_refresh);



}

}


void actualizarMetadata(){




}


int primerConexion() {


return 0;


}
