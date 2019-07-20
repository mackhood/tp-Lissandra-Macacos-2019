/*
 * conexionMemoria.c
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#include "conexionMemoria.h"

void handler_conexion_memoria(t_kernel* tKernel)
{
	while(!destProtocol)
	{
		int conexion=conectar_a_memoria_flexible(configuracion->ip,configuracion->puerto, KERNEL);
		if (conexion == -3 && tKernel->primerConexion)
		{
			logInfo("fallo conexion");
			destProtocol = 1;
		}
		else if(conexion == -3 && tKernel->primerConexion !=1 )
		{
			pthread_mutex_lock(&memoriasCola);
			memoria * loMemoria = queue_pop(tKernel->memoriasCola);
			queue_push(tKernel->memoriasCola,loMemoria);
			pthread_mutex_unlock(&memoriasCola);
			if(loMemoria != NULL )
			{
				if(t_Criterios->strongConsistency->numeroMemoria == configuracion->numeroMemoria)
				{
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
				if(list_any_satisfy(t_Criterios->eventualConsistency->elements,(void*) estaEnLista) )
				{
					pthread_mutex_lock(&ec);
					list_remove_by_condition(t_Criterios->eventualConsistency->elements,(void*)estaEnLista);
					pthread_mutex_unlock(&ec);

				}
				pthread_mutex_lock(&memoriaConfig);
				configuracion = loMemoria;
				pthread_mutex_unlock(&memoriaConfig);

			}
			else
			{
				destProtocol = 1;
			}
		}
		else
		{


			tKernel->primerConexion =0;
			logInfo("Se conecto a una memoria de configuracion");




			prot_enviar_mensaje(conexion,DESCRIBE_REQ,0,NULL);

			t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(conexion);




			//PRIMERO DEBO PERDIR LA CANTIDAD DE MEMORIASCONECTADAS AL POOL DE MEMORIA

			//			t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(conexion);


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

				bool  estaEnMetadataStruct(t_tabla* tablaAux) {
					return  !strcmp(nuevaTabla->nombre, tablaAux->nombre);
				}
				if(!list_any_satisfy(tMetadata->tablas,(void*)estaEnMetadataStruct))
					list_add(tMetadata->tablas, nuevaTabla);
			}
			close(conexion);

			if(mensaje_recibido->head == FULL_DESCRIBE){}
			else if(mensaje_recibido->head == FAILED_DESCRIBE)
			{
				printf("No había ninguna tabla en el FS\n");
			}
			else
			{
				puts("Falló el DESCRIBE global");
			}
			int conexion2=conectar_a_memoria_flexible(configuracion->ip,configuracion->puerto, KERNEL);
			//
			//
			//
			prot_enviar_mensaje(conexion2,GOSSIPING,0,NULL);

			t_prot_mensaje* mensajeAltoque = prot_recibir_mensaje(conexion2);

			char* tablaGossip;
			int large ;
			memcpy(&large,mensajeAltoque->payload,sizeof(int));
			tablaGossip = malloc(large + 1);
			memcpy(tablaGossip,mensajeAltoque->payload+sizeof(int),large);
			tablaGossip[large]=	'\0';
			close(conexion2);

			char ** contenidoTabla = string_split(tablaGossip, ";");
			int a;
			for (a=0; contenidoTabla[a] != NULL ; a++ ){


				char ** infoMemoria = string_split(contenidoTabla[a],",");
				char * ip = string_duplicate(infoMemoria[0]);
				int puerto = atoi(string_duplicate(infoMemoria[1]));
				int numeroMemoriaGeneral = atoi(string_duplicate(infoMemoria[2]));

				bool  estaEnLista2(memoria* memoriaAux) {
					return  string_equals_ignore_case(ip, memoriaAux->ip) &&  (memoriaAux->puerto == puerto) ;
				}

				if(!list_any_satisfy(tKernel->memoriasCola->elements,(void*)estaEnLista2)){

					memoria* laNuevisima = (memoria*)crearMemoria((char*)ip,puerto,numeroMemoriaGeneral);
					pthread_mutex_lock(&memoriasSinCriterio);
					list_add(tKernel->memoriasSinCriterio,laNuevisima);
					pthread_mutex_unlock(&memoriasSinCriterio);

					pthread_mutex_lock(&memoriasCola);
					queue_push(tKernel->memoriasCola,laNuevisima);
					pthread_mutex_unlock(&memoriasCola);
					printf("La memoria de puerto %i e ip %s fue asignada el numero de memoria %i\n",
							laNuevisima->puerto,
							laNuevisima->ip,
							laNuevisima->numeroMemoria);



				}

			}


			usleep(tKernel->config->metadata_refresh*10000);
		}

	}

}

memoria* crearMemoria(char* ip,int puerto,int numeroMemoriaGeneral){

	memoria* nuevaMemoria = malloc(sizeof(memoria));
	nuevaMemoria->puerto = puerto;
	nuevaMemoria->estaEjecutando =0;
	nuevaMemoria->ip = ip;
	estadisticas * estructuraSC = malloc(sizeof(estadisticas));
	estructuraSC->Read_Latency = 0;
	estructuraSC->Reads = 0;
	estructuraSC->Write_Latency = 0;
	estructuraSC->Writes = 0;
	estadisticas * estructuraSHC = malloc(sizeof(estadisticas));
	estructuraSHC->Read_Latency = 0;
	estructuraSHC->Reads = 0;
	estructuraSHC->Write_Latency = 0;
	estructuraSHC->Writes = 0;
	estadisticas * estructuraEC = malloc(sizeof(estadisticas));
	estructuraEC->Read_Latency = 0;
	estructuraEC->Reads = 0;
	estructuraEC->Write_Latency = 0;
	estructuraEC->Writes = 0;
	nuevaMemoria->estadisticasMemoriaSC = estructuraSC;
	nuevaMemoria->estadisticasMemoriaSHC = estructuraSHC;
	nuevaMemoria->estadisticasMemoriaEC = estructuraEC;
	nuevaMemoria->insertsTotales=0;
	nuevaMemoria->selectTotales=0;
	nuevaMemoria->numeroMemoria = numeroMemoriaGeneral;
	return nuevaMemoria;
}

int primerConexion()
{
	return 0;
}
