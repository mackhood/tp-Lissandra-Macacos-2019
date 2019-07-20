/*
 * adminColasPlanificacion.c
 *
 *  Created on: 16 abr. 2019
 *      Author: utnso
 */


#include "adminColasPlanificacion.h"

void initConfigAdminColas(){
	logInfo("Incializando colas de planificacion");
	tKernelEstados = malloc(sizeof(t_kernel_estado));
	tKernelEstados->new = queue_create();
	tKernelEstados->ready = queue_create();
	tKernelEstados->exec = list_create();
	tKernelEstados->exit = queue_create();
	tablaGDT = list_create();
	//inicializacion de mutex para cada lista
	pthread_mutex_init(&mutexListNuevo, NULL);
	pthread_mutex_init(&mutexListListo, NULL);
	pthread_mutex_init(&mutexListFinalizado, NULL);
	pthread_mutex_init(&mutexListEjecucion, NULL);
	pthread_mutex_init(&shc,NULL);
	pthread_mutex_init(&sc,NULL);
	pthread_mutex_init(&ec,NULL);
	pthread_mutex_init(&memoriasCola,NULL);
	pthread_mutex_init(&memoriasCriterio,NULL);
	pthread_mutex_init(&memoriasSinCriterio,NULL);
	pthread_mutex_init(&memoriaConfig,NULL);
	sem_init(&semPCbNew,0,0);
	sem_init(&semPcbReady,0,0);
	sem_init(&semPcbRunning,0,tKernel->config->multiprocesamiento);

}
/**
 * Envio a la cola de NEW e incremento el
 * semaforo contador de DTBs en NEW
 */
void enviarANew(DTB_KERNEL* dtb)
{
	pthread_mutex_lock(&mutexListNuevo);
	queue_push(tKernelEstados->new, dtb);
	signalDTBNew();
	pthread_mutex_unlock(&mutexListNuevo);
	//logInfo("El DTB id %d y path %s ingreso a la cola de NEW",dtb->idGDT, dtb->path);
}


/**
 * Obtengo el dtb de la cola de NEW
 * Decremento el semaforo contador para DTBs
 * en la cola de NEW
 */
DTB_KERNEL*  getDTBNew()
{
	DTB_KERNEL* dtb;

	waitDTBNew(); //descremento semaforo, se aumenta cuando envio un dtb nuevo a new

	pthread_mutex_lock(&mutexListNuevo);
	dtb =  queue_pop(tKernelEstados->new);
	pthread_mutex_unlock(&mutexListNuevo);
	logTrace("programa-dtb  seleccionado de la lista de new :  %d",dtb->idGDT) ;
	return dtb;
}


void enviarAReady(DTB_KERNEL* dtb)
{
	pthread_mutex_lock(&mutexListListo);
	queue_push(tKernelEstados->ready, dtb);
	signalDTBReady();
	pthread_mutex_unlock(&mutexListListo);
	//logInfo("Ingreso el proceso %d a la cola de READY",dtb->idGDT);
}

/**
 * Obtengo el dtb de la cola de ready
 * Decremento el contador de DTBs en ready
 */
DTB_KERNEL* getDTBReady()
{
	DTB_KERNEL* dtb ;
	waitDTBReady();
	pthread_mutex_lock(&mutexListListo);
	dtb = queue_pop(tKernelEstados->ready);
	pthread_mutex_unlock(&mutexListListo);
//	logTrace("programa-dtb :%d seleccionado de la lista de ready ",dtb->idGDT) ;
	return dtb;

}

void enviarAEjecutar(DTB_KERNEL* dtb)
{
	pthread_mutex_lock(&mutexListEjecucion);
	list_add(tKernelEstados->exec,dtb);
	pthread_mutex_unlock(&mutexListEjecucion);
//	logInfo("Se envio el proceso a ejecutar :%d", dtb->idGDT);
}

int getEstadoPlanficacion(DTB_KERNEL* dtb)
{

	bool encontrarDTB(void* element) {
		DTB_KERNEL* dtbTemp = element;
		return dtbTemp->flag == dtb->flag;
	}
	if (list_find(tKernelEstados->new->elements, encontrarDTB) != NULL){
		return ESTADO_COLA_NEW;
	}

	if (list_find(tKernelEstados->ready->elements, encontrarDTB) != NULL)
		return ESTADO_COLA_READY;

	if (list_find(tKernelEstados->exec, encontrarDTB) != NULL)
		return ESTADO_COLA_EXEC;

	if (list_find(tKernelEstados->exit->elements, encontrarDTB) != NULL)
		return ESTADO_COLA_EXIT;

	return ESTADO_NO_ENCONTRADO;
}



DTB_KERNEL* get_elem_new_by_id(int id)
{
	bool encontrar(void* element) {
		DTB_KERNEL* dtbTemp = element;
		return dtbTemp->idGDT == id;
	}

	if(list_any_satisfy(tKernelEstados->new->elements, encontrar)){
		pthread_mutex_lock(&mutexListNuevo);
		DTB_KERNEL*  dtb = list_find(tKernelEstados->new->elements, encontrar);
		pthread_mutex_unlock(&mutexListNuevo);
		return dtb;

	}else
		return NULL;

}

void enviarAEXIT(DTB_KERNEL* dtb) {
	pthread_mutex_lock(&mutexListFinalizado);
	queue_push(tKernelEstados->exit, dtb);
	pthread_mutex_unlock(&mutexListFinalizado);
	logTrace("Ingreso a la cola de EXIT el DTB :%i", dtb->idGDT);

}

void moverExecToReady(DTB_KERNEL* dtb)
{
	removerDeExec(dtb);
	usleep(tKernel->config->sleep_ejecucion*1000);
	enviarAReady(dtb);
}

void removerDeExec(DTB_KERNEL* dtb)
{
	bool predicado(void* d)
	{
		DTB_KERNEL* dtbTemp = d;
		return dtbTemp->idGDT == dtb->idGDT;
	}
	list_remove_by_condition(tKernelEstados->exec, predicado);
	logTrace("El dtb %d salio de la lista de EXEC", dtb->idGDT);
}

void removerDeReady(DTB_KERNEL* dtb)
{

	bool condicionDTB(DTB_KERNEL* dtbTemp) {;
		return dtbTemp->idGDT == dtb->idGDT;
	}
	list_remove_by_condition(tKernelEstados->ready->elements, (void*)condicionDTB);
	signalDTBReady();
	logTrace("El DTB:%d se removio de la cola de ready", dtb->idGDT);
}

void removerDeNuevo(DTB_KERNEL* dtb)
{
	bool condicionDTB(DTB_KERNEL* dtbTemp) {;
		return dtbTemp->idGDT == dtb->idGDT;
	}
	list_remove_by_condition(tKernelEstados->new->elements,(void*)condicionDTB);
	logTrace("El DTB:%d se removio de la cola de new", dtb->idGDT);
}

DTB_KERNEL* buscar_pcb_exec_by_id( int pid ){

	DTB_KERNEL* pcb;
	bool predicado(void* d) {
		DTB_KERNEL* dtbTemp = d;
			return dtbTemp->idGDT == pid;
	}
	pthread_mutex_lock(&mutexListEjecucion);
	pcb = list_find(tKernelEstados->exec, predicado);
	pthread_mutex_unlock(&mutexListEjecucion);
	if(pcb==NULL){
	 logError("El dtb id %d no esta en ejecucion", pid);
	 return pcb;
	}

	return pcb;

}


void signalDTBNew(){
	sem_post(&semPCbNew);
}

void waitDTBNew(){
	sem_wait(&semPCbNew);
}

void signalDTBReady(){
	sem_post(&semPcbReady);
}

void waitDTBReady(){
	sem_wait(&semPcbReady);
}

void waitDTBRunning()
{
	sem_wait(&semPcbRunning);
}

void signalDTBRunning()
{
	sem_post(&semPcbRunning);
}

void moverExecToExit(DTB_KERNEL* dtb)
{
	removerDeExec(dtb);
	enviarAEXIT(dtb);
}

void moverNewToExit(DTB_KERNEL* dtb)
{
	removerDeNuevo(dtb);
	enviarAEXIT(dtb);
}

void moverNewToReady(DTB_KERNEL* dtb)
{
	removerDeNuevo(dtb);
	enviarAReady(dtb);
}

void moverReadyToExit(DTB_KERNEL* dtb)
{
	removerDeReady(dtb);
	enviarAEXIT(dtb);
}

void moverReadyToExec(DTB_KERNEL* dtb)
{
	removerDeReady(dtb);
	enviarAEjecutar( dtb );
}


DTB_KERNEL* crearDTBKernel(int gdtId, char* path, int quantum){
	DTB_KERNEL* dtb = malloc( sizeof(DTB_KERNEL) );
	dtb->idGDT = gdtId;
	dtb->flag = 0;
	dtb->horacreacion = time(0);
	dtb->quantum = quantum;
	dtb->tablaSentenciasMejorada = queue_create();
	return dtb;
}

void liberarMemoriaDTB_SAFA(DTB_KERNEL* dtb ){
		free(dtb->sentencias);
		free(dtb);
}

