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
	//t_list* pendReady ;	  //Aca paso los dtb que envie a cargar en memoria y esta pendiente de pasar a ready
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

sem_t semPcbRunning;





void initConfigAdminColas();
void enviarANew(DTB_KERNEL* dtb);
void enviarAColaPrioridad(DTB_KERNEL* dtb);
DTB_KERNEL* getDTBColaPrioridad();
DTB_KERNEL*  getDTBNew();
void enviarAReady(DTB_KERNEL* dtb);
DTB_KERNEL* getDTBReady();
void enviarAEjecutar(DTB_KERNEL* dtb);
int getEstadoPlanficacion(DTB_KERNEL* dtb);
void enviarABLOCK(DTB_KERNEL* dtb);
DTB_KERNEL* get_elem_new_by_id(int id);
void enviarAEXIT(DTB_KERNEL* dtb);
DTB_KERNEL* getDTB_DUMMYofBLOCK();
void moverExecToReady(DTB_KERNEL* dtb);
void removerDeExec(DTB_KERNEL* dtb);
void removerDeBLOCK(DTB_KERNEL* dtb);
void removerDeReady(DTB_KERNEL* dtb);
void removerDeNuevo(DTB_KERNEL* dtb);
DTB_KERNEL* buscar_pcb_exec_by_id( int pid );
void signalDTBNew();
void waitDTBNew();
void signalDTBReady();
void waitDTBReady();
void signalDTBReadyPrioridad();
void waitDTBReadyPrioridad();
void signalDTBRunning();
void waitDTBRunning();
void moverExecToBlock(DTB_KERNEL* dtb);
void moverExecToExit(DTB_KERNEL* dtb);
void moverBlockToExit(DTB_KERNEL* dtb);
void moverBlockToReady(DTB_KERNEL* dtb);
void moverNewToExit(DTB_KERNEL* dtb);
void moverNewToReady(DTB_KERNEL* dtb);
void moverReadyToExit(DTB_KERNEL* dtb);
void moverReadyToExec(DTB_KERNEL* dtb);
DTB_KERNEL* crearDTBKernel(int gdtId, char* path, int quantum);
void liberarMemoriaDTB_SAFA(DTB_KERNEL* dtb );






#endif /* ADMINCOLASPLANIFICACION_H_ */
