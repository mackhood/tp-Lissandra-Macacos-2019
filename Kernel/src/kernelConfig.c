/*
 * kernelConfig.c
 *
 *  Created on: 2 abr. 2019
 *      Author: utnso
 */


#include "kernelConfig.h"



GDT* crearGdt(int idGdt){
	GDT* gdt = (GDT*)malloc( sizeof(GDT) );
	gdt->idGDT = idGdt;
	return gdt;
}
void agregarGDT(GDT* gdt){
	pthread_mutex_lock(&mutextListGDT);
	list_add(tablaGDT, gdt);
	pthread_mutex_unlock(&mutextListGDT);
	logInfo("Se crea GDT: %d", gdt->idGDT);
}




void actualizarGDT(GDT* gdt){

	removeGDT( gdt );
	pthread_mutex_lock(&mutextListGDT);
	list_add(tablaGDT, gdt);
	pthread_mutex_unlock(&mutextListGDT);
	logInfo("se actualiza: %d", gdt->idGDT);
}

void removeGDT(GDT* gdt){
	bool predicado(void* element) {
		GDT* gdtTemp = element;
		return gdtTemp->idGDT == gdt->idGDT;
	}

	pthread_mutex_lock(&mutextListGDT);
	GDT* gdt_borrar = list_remove_by_condition(tablaGDT, (void*)predicado);
	pthread_mutex_unlock(&mutextListGDT);
	logInfo("Se buscó gdt  id: %d", gdt->idGDT);

	destroyGdt(gdt_borrar);


}

void destroyGdt(GDT* gdt){
	free(gdt);
}

GDT* buscar_gdt_by_id(int pid){
	bool predicado(void* element) {
		GDT* gdtTemp = element;
		return gdtTemp->idGDT == pid;
	}
	pthread_mutex_lock(&mutextListGDT);

	GDT* gdt = list_find(tablaGDT, (void*)predicado);

	pthread_mutex_unlock(&mutextListGDT);
	logInfo("Se buscó gdt  id: %d", gdt->idGDT);

	return gdt;
 }

bool  estaEnMetadata(char *nombre_tabla) {

	bool _esLaBuscada(t_tabla* tabla){
	return  !strcmp(tabla->nombre,nombre_tabla);
		}



return	list_any_satisfy(tMetadata->tablas,(void*)_esLaBuscada);



}






