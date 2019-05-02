/*
 * InicializacionPcb.c
 *
 *  Created on: 30 abr. 2019
 *      Author: utnso
 */


#include "InicializacionPcb.h"

	int actualIdGDT = 100 ;

	int getIdGDT(){
			int id;
			pthread_mutex_lock(&mutexIdGDT);
			id = actualIdGDT++;
			pthread_mutex_unlock(&mutexIdGDT);
			return id;
		}


		void destruir_dtb(DTB_KERNEL* pcb) {
			free(pcb->path);
			free(pcb);
		}




		void initNuevoGDT(char* pathScriptEscriptorio){

			int idGdt = getIdGDT();
			agregarGDT( crearGdt(idGdt) );
			//enviarANew(crearDTBKERNEL(idGdt, pathScriptEscriptorio, tKernel->config->quantum));

		}





		void liberarDTB(DTB_KERNEL* dtb){
			free(dtb->path);
//			free(dtb->tablaDeArchivosAbiertos);
//			free(dtb->tablaDeArchivosAbiertos);
//
			}



		//ESTO SE ENCUENTRA EN DESARROLLO POR FAVOR NO BORRAR


//		void mostrarStatus(int pid){
//
//			bool existe(DTB_SAFA* element){
//				return element->idGDT == pid;
//			}
//
//			bool estaAqui;
//			DTB_SAFA* dtb;
//
//			pthread_mutex_lock(&mutexListNuevo);
//			estaAqui = list_any_satisfy( tSafaEstados->new->elements, (void*)existe);
//			if(estaAqui){
//				dtb = list_find( tSafaEstados->new->elements, (void*)existe);
//				printf("ESTADO: NEW  PID: %d  FLAG: %d  PATH: %s \n", dtb->idGDT, dtb->flag, dtb->path);
//				pthread_mutex_unlock(&mutexListNuevo);
//				return;
//			}
//			pthread_mutex_unlock(&mutexListNuevo);
//
//			pthread_mutex_lock(&mutexListFinalizado);
//			estaAqui = list_any_satisfy( tSafaEstados->exit->elements, (void*)existe);
//			 if(estaAqui){
//				dtb = list_find( tSafaEstados->exit->elements, (void*)existe);
//				printf("ESTADO: EXIT  PID: %d  FLAG: %d  PATH: %s \n", dtb->idGDT, dtb->flag, dtb->path);
//				pthread_mutex_unlock(&mutexListFinalizado);
//				return;
//			 }
//			pthread_mutex_unlock(&mutexListFinalizado);
//
//
//			pthread_mutex_lock(&mutexListListo);
//			estaAqui = list_any_satisfy(tSafaEstados->ready->elements, (void*)existe);
//			if(estaAqui){
//				dtb = list_find(tSafaEstados->ready->elements, (void*)existe);
//				printf("ESTADO: READY  PID: %d  FLAG: %d  PATH: %s \n", dtb->idGDT, dtb->flag, dtb->path);
//				pthread_mutex_unlock(&mutexListListo);
//				return;
//			}
//			pthread_mutex_unlock(&mutexListListo);
//
//
//			pthread_mutex_lock(&mutexListEjecucion);
//			estaAqui = list_any_satisfy(tSafaEstados->exec, (void*)existe);
//			if(estaAqui){
//				dtb = list_find(tSafaEstados->exec, (void*)existe);
//				printf("ESTADO: EXEC  PID: %d  FLAG: %d  PATH: %s \n", dtb->idGDT, dtb->flag, dtb->path);
//				pthread_mutex_unlock(&mutexListEjecucion);
//				return;
//			}
//			pthread_mutex_unlock(&mutexListEjecucion);
//
//
//			pthread_mutex_lock(&mutexListBloqueado);
//			estaAqui = list_any_satisfy(tSafaEstados->block, (void*)existe);
//			if(estaAqui){
//				dtb = list_find(tSafaEstados->block, (void*)existe);
//				printf("ESTADO: BLOCK  PID: %d  FLAG: %d  PATH: %s \n", dtb->idGDT, dtb->flag, dtb->path);
//				pthread_mutex_unlock(&mutexListBloqueado);
//				return;
//			}
//			pthread_mutex_unlock(&mutexListBloqueado);
//
//
//			pthread_mutex_lock(&mutexListPendReady);
//			estaAqui = list_any_satisfy(tSafaEstados->pendReady, (void*)existe);
//			if(estaAqui){
//				dtb = list_find(tSafaEstados->pendReady, (void*)existe);
//				printf("ESTADO: PEN-READY  PID: %d  FLAG: %d  PATH: %s \n", dtb->idGDT, dtb->flag, dtb->path);
//				pthread_mutex_unlock(&mutexListPendReady);
//				return;
//			}
//				pthread_mutex_unlock(&mutexListPendReady);
//
//			}
//
//
//
//
//		void finalizarById(int pid){
//
//			bool existe(DTB_SAFA* element){
//				return element->idGDT == pid;
//			}
//
//			bool estaAqui;
//			DTB_SAFA* dtb;
//
//			pthread_mutex_lock(&mutexListNuevo);
//			estaAqui = list_any_satisfy( tSafaEstados->new->elements, (void*)existe);
//			if(estaAqui){
//				dtb = (DTB_SAFA*)list_find( tSafaEstados->new->elements, (void*)existe);
//				moverNewToExit(dtb);
//				pthread_mutex_unlock(&mutexListNuevo);
//				return;
//			}
//			pthread_mutex_unlock(&mutexListNuevo);
//
//			pthread_mutex_lock(&mutexListFinalizado);
//			estaAqui = list_any_satisfy( tSafaEstados->exit->elements, (void*)existe);
//			if(estaAqui){
//				dtb = list_find(tSafaEstados->ready->elements, (void*)existe);
//				printf("El DTB con pid = %d ya está finalizado \n", dtb->idGDT);
//				pthread_mutex_unlock(&mutexListFinalizado);
//				return;
//			}
//
//			pthread_mutex_unlock(&mutexListFinalizado);
//
//
//			pthread_mutex_lock(&mutexListListo);
//			estaAqui = list_any_satisfy(tSafaEstados->ready->elements, (void*)existe);
//			if(estaAqui){
//				dtb = list_find(tSafaEstados->ready->elements, (void*)existe);
//				moverReadyToExit(dtb);
//				pthread_mutex_unlock(&mutexListListo);
//				return;
//			}
//			pthread_mutex_unlock(&mutexListListo);
//
//
//			pthread_mutex_lock(&mutexListEjecucion);
//			estaAqui = list_any_satisfy(tSafaEstados->exec, (void*)existe);
//			if(estaAqui){
//				dtb = list_find(tSafaEstados->exec, (void*)existe);
//				moverExecToExit(dtb);
//				pthread_mutex_unlock(&mutexListEjecucion);
//				return;
//			}
//			pthread_mutex_unlock(&mutexListEjecucion);
//
//			pthread_mutex_lock(&mutexListBloqueado);
//				estaAqui = list_any_satisfy(tSafaEstados->block, (void*)existe);
//				if(estaAqui){
//					dtb = list_find(tSafaEstados->block, (void*)existe);
//					moverBlockToExit(dtb);
//					pthread_mutex_unlock(&mutexListBloqueado);
//					return;
//				}
//				pthread_mutex_unlock(&mutexListBloqueado);
//
//
//				pthread_mutex_lock(&mutexListPendReady);
//				estaAqui = list_any_satisfy(tSafaEstados->pendReady, (void*)existe);
//				if(estaAqui){
//					dtb = list_find(tSafaEstados->pendReady, (void*)existe);
//					moverPenReadyToExit(dtb);
//					pthread_mutex_unlock(&mutexListPendReady);
//					return;
//				}
//				pthread_mutex_unlock(&mutexListPendReady);
//
//			}
//
//
//
//
//
//
//
//
