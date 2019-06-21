/*
 * conexionMemoria.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "conexionMemoria.h"

void handler_conexion_memoria(t_kernel* tKernel) {

	while(!destProtocol){

		pthread_mutex_lock(&configuracion->enUso);
		int conexion=conectar_a_memoria_flexible(configuracion->ip,configuracion->puerto,"Memoria1");

		if (conexion ==3 && tKernel->primerConexion){

			logInfo("fallo conexion");

			destProtocol = 1;





		}else if(conexion ==3 && tKernel->primerConexion !=1 ){

			pthread_mutex_lock(&memoriasCola);
			memoria * loMemoria = queue_pop(tKernel->memoriasCola);
			pthread_mutex_unlock(&memoriasCola);

			if(loMemoria != NULL ){

				if(t_Criterios->strongConsistency->numeroMemoria == configuracion->numeroMemoria){
					pthread_mutex_lock(&sc);
					t_Criterios->strongConsistency = loMemoria;
				}


				bool  estaEnLista(memoria* memoriaAux) {
					return  configuracion->numeroMemoria == memoriaAux->numeroMemoria;
				}

				if(list_any_satisfy(t_Criterios->StrongHash,(void*) estaEnLista) ){
					pthread_mutex_lock(&shc);
					list_remove_by_condition(t_Criterios->StrongHash,(void*)estaEnLista);
					pthread_mutex_unlock(&shc);

				}



				if(list_any_satisfy(t_Criterios->eventualConsistency->elements,(void*) estaEnLista) ){
					pthread_mutex_lock(&ec);
					list_remove_by_condition(t_Criterios->eventualConsistency->elements,(void*)estaEnLista);
					pthread_mutex_unlock(&ec);

				}

				pthread_mutex_lock(&memoriaConfig);

				configuracion = loMemoria;
				pthread_mutex_unlock(&memoriaConfig);

			}else {


				destProtocol = 1;



			}
		}else {



			logInfo("Se conecto a una memoria de configuracion");




			prot_enviar_mensaje(conexion,DESCRIBE_REQ,0,NULL);

			t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(conexion);




			//PRIMERO DEBO PERDIR LA CANTIDAD DE MEMORIASCONECTADAS AL POOL DE MEMORIA

			//	t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(conexion);


			int tamanio ;
			memcpy(&tamanio,mensaje_recibido->payload,sizeof(int));

			char * mensaje = malloc(tamanio + 1);
			memcpy(mensaje,mensaje_recibido->payload + sizeof(int), tamanio);
			mensaje[tamanio] = '\0';
			char ** tablaDescribe = string_split(mensaje, ";");


			int k=0;
			list_clean(tMetadata->tablas);
			while(tablaDescribe[k] != NULL  ){

				char* tabla = strtok(tablaDescribe[k],",");
				char* criterio = strtok(NULL, ",");
				k++;
				t_tabla * nuevaTabla = malloc(sizeof(t_tabla));
				nuevaTabla->nombre = tabla;
				nuevaTabla->criterio = criterio;

				bool  estaEnMetadata(t_tabla* tablaAux) {
					return  string_equals_ignore_case(nuevaTabla->nombre, tablaAux->nombre);
				}


				if(!list_any_satisfy(tMetadata->tablas,(void*)estaEnMetadata))
					list_add(tMetadata->tablas, nuevaTabla);





			}








			prot_enviar_mensaje(conexion,GOSSIPING,0,NULL);

			t_prot_mensaje* mensajeAltoque = prot_recibir_mensaje(conexion);

			char * tablaGossip ;
			int large ;
			memcpy(&tamanio,mensajeAltoque->payload,sizeof(int));
			tablaGossip = malloc(large + 1);
			memcpy(tablaGossip,mensaje_recibido->payload+sizeof(int),large);
			tablaGossip[large]=	'\0';
			close(conexion);

			char ** contenidoTabla = string_split(tablaGossip, ";");
			int a;
			for (a=0; contenidoTabla[a] != NULL ; a++ ){


				char ** infoMemoria = string_split(contenidoTabla[a],",");
				char * ip = string_duplicate(infoMemoria[0]);
				int puerto = atoi(string_duplicate(infoMemoria[1]));

				bool  estaEnLista2(memoria* memoriaAux) {
				return  string_equals_ignore_case(ip, memoriaAux->ip) &&  (memoriaAux->puerto == puerto) ;
				}

				if(!list_any_satisfy(tKernel->memoriasCola->elements,(void*)estaEnLista2)){

				memoria * laNuevisima = crearMemoria(ip,puerto);
				pthread_mutex_lock(&memoriasSinCriterio);
				list_add(tKernel->memoriasSinCriterio,laNuevisima);
				pthread_mutex_unlock(&memoriasSinCriterio);

				pthread_mutex_lock(&memoriasCola);
				queue_push(tKernel->memoriasCola,laNuevisima);
				pthread_mutex_unlock(&memoriasCola);




				}

			}




			usleep(tKernel->config->metadata_refresh*1000);

		}

	}

}


void actualizarMetadata(){




}


int primerConexion() {


	return 0;


}
