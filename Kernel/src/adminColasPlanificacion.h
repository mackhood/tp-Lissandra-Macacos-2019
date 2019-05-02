/*
 * adminColasPlanificacion.h
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */

#ifndef ADMINCOLASPLANIFICACION_H_
#define ADMINCOLASPLANIFICACION_H_


#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/collections/dictionary.h>
#include <pthread.h>
#include <strings.h>
#include <semaphore.h>
#include <stddef.h>
#include <time.h>
#include "kernelConfig.h"

#define ESTADO_COLA_NEW   		1
#define ESTADO_COLA_READY   	2
#define ESTADO_COLA_EXEC   		3
#define ESTADO_COLA_EXIT    	5
#define ESTADO_NO_ENCONTRADO   -1


DTB_KERNEL* crearDTBSAFA(int gdtId, char* path, int quantum);


typedef struct {
	t_queue* new;			  //aca van los DTB asociado al programa cundo el usuario ejecuta un script
	t_list* pendReady ;	  //Aca paso los dtb que envie a cargar en memoria y esta pendiente de pasar a ready
	t_queue* ready;
    t_list* exec;
	t_queue* exit;
}t_kernel_estado;





t_kernel_estado* tKernelEstados;








pthread_mutex_t mutexListNuevo;
pthread_mutex_t mutexListListo;
pthread_mutex_t mutexListFinalizado;
pthread_mutex_t mutexListBloqueado;
pthread_mutex_t mutexListEjecucion;
pthread_mutex_t mutexListPendReady;
pthread_mutex_t mutexListcolaPrioridad;
sem_t semPCbNew ;
sem_t semPcbReady;
sem_t semPcbReadyPrioridad;












































#endif /* ADMINCOLASPLANIFICACION_H_ */
