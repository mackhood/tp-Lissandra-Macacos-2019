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
//	tKernelEstados->pendReady = list_create();
	tKernelEstados->ready = queue_create();
//	tKernelEstados->colaPrioridad = queue_create();
	tKernelEstados->exec = list_create();
	tKernelEstados->exit = queue_create();
	tablaGDT = list_create();
	tabla_recursos = dictionary_create();
//	tKernelEstados->ListaPrioridad=list_create();
	//inicializacion de mutex para cada lista
	pthread_mutex_init(&mutexListNuevo, NULL);
	pthread_mutex_init(&mutexListListo, NULL);
	pthread_mutex_init(&mutexListFinalizado, NULL);
	pthread_mutex_init(&mutexListBloqueado, NULL);
	pthread_mutex_init(&mutexListEjecucion, NULL);
	pthread_mutex_init(&mutexListPendReady, NULL);
	pthread_mutex_init(&mutexListcolaPrioridad, NULL);
	pthread_mutex_init(&shc,NULL);
	pthread_mutex_init(&sc,NULL);
	pthread_mutex_init(&ec,NULL);
	pthread_mutex_init(&memoriasCola,NULL);
	pthread_mutex_init(&memoriasCriterio,NULL);
	pthread_mutex_init(&memoriasSinCriterio,NULL);
	pthread_mutex_init(&memoriaConfig,NULL);





	sem_init(&semPCbNew,0,0);
	sem_init(&semPcbReady,0,0);
	sem_init(&semPcbReadyPrioridad,0,0);
	sem_init(&semPcbRunning,0,tKernel->config->multiprocesamiento);

}
/**
 * Envio a la cola de NEW e incremento el
 * semaforo contador de DTBs en NEW
 */
void enviarANew(DTB_KERNEL* dtb){
	pthread_mutex_lock(&mutexListNuevo);
	queue_push(tKernelEstados->new, dtb);
	signalDTBNew();
	pthread_mutex_unlock(&mutexListNuevo);
	//logInfo("El DTB id %d y path %s ingreso a la cola de NEW",dtb->idGDT, dtb->path);
}



void enviarAColaPrioridad(DTB_KERNEL* dtb){
	pthread_mutex_lock(&mutexListcolaPrioridad);
	signalDTBReadyPrioridad();
	queue_push(tKernelEstados->new, dtb);
	pthread_mutex_unlock(&mutexListcolaPrioridad);
	logInfo("El DTB id %d y path %s ingreso a la cola de Prioridad",dtb->idGDT, dtb->path);
}

DTB_KERNEL* getDTBColaPrioridad(){
	DTB_KERNEL* dtb ;
	waitDTBReadyPrioridad();
	pthread_mutex_lock(&mutexListListo);
	dtb = queue_pop(tKernelEstados->ready);
	pthread_mutex_unlock(&mutexListListo);
	logTrace("programa-dtb :%d seleccionado de la lista de ready ",dtb->idGDT) ;
	return dtb;

}




/**
 * Obtengo el dtb de la cola de NEW
 * Decremento el semaforo contador para DTBs
 * en la cola de NEW
 */
DTB_KERNEL*  getDTBNew(){
	DTB_KERNEL* dtb;

	waitDTBNew(); //descremento semaforo, se aumenta cuando envio un dtb nuevo a new

	pthread_mutex_lock(&mutexListNuevo);
	dtb =  queue_pop(tKernelEstados->new);
	pthread_mutex_unlock(&mutexListNuevo);
	logTrace("programa-dtb  seleccionado de la lista de new :  %d",dtb->idGDT) ;
	return dtb;
}


/**
 * Envio a la cola PEND_READY
 */
//void enviarAPendReady(DTB_KERNEL* dtb){
//
//	pthread_mutex_lock(&mutexListPendReady);
//	list_add(tKernelEstados->pendReady,dtb);
//	pthread_mutex_unlock(&mutexListPendReady);
//
//	logInfo("Ingreso el proceso a la cola pendiente de ready :%d", dtb->idGDT);
//}

void enviarAReady(DTB_KERNEL* dtb){
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
DTB_KERNEL* getDTBReady(){
	DTB_KERNEL* dtb ;
	waitDTBReady();
	pthread_mutex_lock(&mutexListListo);
	dtb = queue_pop(tKernelEstados->ready);
	pthread_mutex_unlock(&mutexListListo);
//	logTrace("programa-dtb :%d seleccionado de la lista de ready ",dtb->idGDT) ;
	return dtb;

}

void enviarAEjecutar(DTB_KERNEL* dtb) {
	pthread_mutex_lock(&mutexListEjecucion);
	list_add(tKernelEstados->exec,dtb);
	pthread_mutex_unlock(&mutexListEjecucion);
//	logInfo("Se envio el proceso a ejecutar :%d", dtb->idGDT);
}

int getEstadoPlanficacion(DTB_KERNEL* dtb){

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

//	if (list_find(tKernelEstados->block, encontrarDTB) != NULL)
//		return ESTADO_COLA_BLOCK;

	if (list_find(tKernelEstados->exit->elements, encontrarDTB) != NULL)
		return ESTADO_COLA_EXIT;

	return ESTADO_NO_ENCONTRADO;
}

void enviarABLOCK(DTB_KERNEL* dtb) {
	pthread_mutex_lock(&mutexListBloqueado);
//	list_add(tKernelEstados->block, dtb);
	pthread_mutex_unlock(&mutexListBloqueado);
	logTrace(" Se envio a cola de BLoqueados el DTB :%d", dtb->idGDT);
}

//DTB_KERNEL* get_elem_block_by_id(int id){
//	bool encontrar(void* element) {
//		DTB_KERNEL* dtbTemp = element;
//		return dtbTemp->idGDT == id;
//	}

//	if(list_any_satisfy(tKernelEstados->block, encontrar)){
//		pthread_mutex_lock(&mutexListBloqueado);
//		DTB_KERNEL* dtb = list_find(tKernelEstados->block, encontrar);
//		pthread_mutex_unlock(&mutexListBloqueado);
//		return dtb;
//
//	}else
//		return NULL;

//}

DTB_KERNEL* get_elem_new_by_id(int id){
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

//DTB_KERNEL* get_elem_penready_by_id(int id){
//	bool encontrar(void* element) {
//		DTB_KERNEL* dtbTemp = element;
//		return dtbTemp->idGDT == id;
//	}
//
//	if(list_any_satisfy(tKernelEstados->pendReady, encontrar)){
//		pthread_mutex_lock( &mutexListPendReady );
//		DTB_KERNEL*  dtb = list_find(tKernelEstados->pendReady, encontrar);
//		pthread_mutex_unlock( &mutexListPendReady );
//		return dtb;
//
//	}else
//		return NULL;
//
//}

void enviarAEXIT(DTB_KERNEL* dtb) {
	pthread_mutex_lock(&mutexListFinalizado);
	queue_push(tKernelEstados->exit, dtb);
	pthread_mutex_unlock(&mutexListFinalizado);
	logTrace("Ingreso a la cola de EXIT el DTB :%d", dtb->idGDT);

}

/*
 * Obtiene el DTB_DUMMY de la cola de bloqueados
 *
 */
DTB_KERNEL* getDTB_DUMMYofBLOCK(){
	bool condicionDTB_DUMMY(void* element) {
		DTB_KERNEL* dtbTemp = element;
		return dtbTemp->flag == 0;
	}

	DTB_KERNEL* dtb ;
//	pthread_mutex_lock(&mutexListBloqueado);
//	dtb = list_remove_by_condition(tKernelEstados->block, condicionDTB_DUMMY);
//	pthread_mutex_unlock(&mutexListBloqueado);
//
//	if(dtb != NULL){
//		logInfo("Se removio DTB_DUMMY de la cola de bloqueados");
//	}else{
//		logInfo("Error al remover DTB_DUMMY de la cola de bloqueados");
//	}

	return dtb;
}
void moverExecToReady(DTB_KERNEL* dtb) {
	removerDeExec(dtb);
	enviarAReady(dtb);
}

void removerDeExec(DTB_KERNEL* dtb) {
	bool predicado(void* d) {
		DTB_KERNEL* dtbTemp = d;
		return dtbTemp->idGDT == dtb->idGDT;
	}
	list_remove_by_condition(tKernelEstados->exec, predicado);
	logTrace("El dtb %d salio de la lista de EXEC", dtb->idGDT);
}

void removerDeBLOCK(DTB_KERNEL* dtb) {

	bool condicionDTB(DTB_KERNEL* dtbTemp) {;
		return dtbTemp->idGDT == dtb->idGDT;
	}
//	list_remove_by_condition(tKernelEstados->block, (void*)condicionDTB);
	logTrace("El DTB:%d se removio de la cola de bloqueados", dtb->idGDT);
}

//void removerDePenReady(DTB_KERNEL* dtb) {
//
//	bool condicionDTB(DTB_KERNEL* dtbTemp) {;
//		return dtbTemp->idGDT == dtb->idGDT;
//	}
//	list_remove_by_condition(tKernelEstados->pendReady,(void*)condicionDTB);
//	logTrace("El DTB:%d se removio de la cola de pendiente de ready", dtb->idGDT);
//}

void removerDeReady(DTB_KERNEL* dtb) {

	bool condicionDTB(DTB_KERNEL* dtbTemp) {;
		return dtbTemp->idGDT == dtb->idGDT;
	}
	list_remove_by_condition(tKernelEstados->ready->elements, (void*)condicionDTB);
	signalDTBReady();
	logTrace("El DTB:%d se removio de la cola de ready", dtb->idGDT);
}

void removerDeNuevo(DTB_KERNEL* dtb) {
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

void signalDTBReadyPrioridad(){
	sem_post(&semPcbReadyPrioridad);
}

void waitDTBReadyPrioridad(){
	sem_wait(&semPcbReadyPrioridad);
}

void waitDTBReady(){
	sem_wait(&semPcbReady);
}

void waitDTBRunning(){
	sem_wait(&semPcbRunning);
}

void signalDTBRunning(){
	sem_post(&semPcbRunning);
}



void moverExecToBlock(DTB_KERNEL* dtb){
	removerDeExec(dtb);
	enviarABLOCK(dtb);
}

void moverExecToExit(DTB_KERNEL* dtb){
	removerDeExec(dtb);
	enviarAEXIT(dtb);
}

void moverBlockToExit(DTB_KERNEL* dtb){
	removerDeBLOCK(dtb);
	enviarAEXIT(dtb);
}

void moverBlockToReady(DTB_KERNEL* dtb){
	removerDeBLOCK(dtb);
	enviarAReady(dtb);
}

//void moverPenReadyToExit(DTB_KERNEL* dtb){
//	removerDePenReady(dtb);
//	enviarAEXIT(dtb);
//}

void moverNewToExit(DTB_KERNEL* dtb){
	removerDeNuevo(dtb);
	enviarAEXIT(dtb);
}

void moverNewToReady(DTB_KERNEL* dtb){
	removerDeNuevo(dtb);
	enviarAReady(dtb);
}

//void moverPendienteReadyToReady(DTB_KERNEL* dtb){
//	removerDePenReady( dtb );
//	enviarAReady(dtb);
//}

void moverReadyToExit(DTB_KERNEL* dtb){
	removerDeReady(dtb);
	enviarAEXIT(dtb);
}

void moverReadyToExec(DTB_KERNEL* dtb){
	removerDeReady(dtb);
	enviarAEjecutar( dtb );
}


DTB_KERNEL* crearDTBKernel(int gdtId, char* path, int quantum){
	DTB_KERNEL* dtb = malloc( sizeof(DTB_KERNEL) );
	dtb->idGDT = gdtId;
	dtb->flag = 0;
	dtb->pc = 0;
//	dtb->cant_usaron_diego = 0;
	dtb->se_ejecuto = false;
	dtb->horacreacion = time(0);
	dtb->quantum = quantum;
//	dtb->path = malloc( (sizeof(char)* string_length(path)) + 1 );
//	strcpy(dtb->path, path);
//	dtb->tablaDeArchivosAbiertos = list_create();
//	dtb->prox_io = queue_create();
	//dtb->tablaSentencias;
	dtb->tablaSentenciasMejorada = queue_create();
	return dtb;
}

void liberarMemoriaDTB_SAFA(DTB_KERNEL* dtb ){


		free(dtb->sentencias);
		free(dtb->path);
//		queue_destroy( dtb->prox_io );
//		list_destroy(dtb->tablaDeArchivosAbiertos);
		free(dtb);

}

//void desbloquear_esperando_recurso( t_recurso* rec ){
//
//	if(!queue_is_empty(rec->en_espera)){
//		for(int i=rec->valor; i>0;i--){
//			GDT* gdt = queue_pop(rec->en_espera);
//			DTB_KERNEL* dtb = get_elem_block_by_id( gdt->idGDT );
//			moverBlockToReady(dtb);
//		}
//	}
//}



























