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
	gdt->recursos = list_create();
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
	list_destroy(gdt->recursos);
	free(gdt);
}

void destroyrecurso(t_recurso* rec){

	queue_destroy(rec->en_espera);
	free( rec->recurso );
	free( rec );
}


t_recurso* crear_recurso(char* recurso){
	t_recurso* r = malloc( sizeof(t_recurso) );
	r->recurso = malloc( string_length(recurso) + 1 );
	memcpy(r->recurso, recurso, string_length(recurso) );
	r->valor = 1;
	r->en_espera = queue_create();
	return r;

}

t_archivoAbierto* crear_archivo_abierto(char* path, int posMemoria){
	t_archivoAbierto* arch = malloc( sizeof(t_archivoAbierto) );
	arch->path = malloc( string_length( path ) + 1);
	memcpy(arch->path, path, string_length( path ) + 1 );
	arch->posMemoria = posMemoria;

	return arch;
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


void crear_recurso_en_sistema(char* recurso, t_recurso* rec){
	pthread_mutex_lock(&mutextListRecurso);
	dictionary_put(tabla_recursos, recurso, rec);
	pthread_mutex_unlock(&mutextListRecurso);
	logInfo("se  creo recurso %s", recurso);
}

void actualizar_recurso(char* recurso, t_recurso* rec){

	t_recurso* rec_borrar =	extraer_recurso( recurso );
	crear_recurso_en_sistema(recurso, rec);
	destroyrecurso( rec_borrar );
	logInfo("Se actualizó recurso %s", recurso);
}


t_recurso* extraer_recurso(char* recurso){
	pthread_mutex_lock(&mutextListRecurso);
	t_recurso* rec = dictionary_remove(tabla_recursos, recurso);
	pthread_mutex_lock(&mutextListRecurso);
	logInfo("se  extrajo recurso %s", rec->recurso);
	return rec;

}

t_recurso* get_recurso(char* recurso){
	pthread_mutex_lock(&mutextListRecurso);
	t_recurso* rec = dictionary_get(tabla_recursos, recurso);
	pthread_mutex_lock(&mutextListRecurso);
	logInfo("se  extrajo recurso %s", rec->recurso);
	return rec;
}

void asigna_recurso(t_recurso* rec, GDT* gdt){

	list_add(gdt->recursos, rec);
	logInfo("se  agrega recurso %s a gdt %d", rec->recurso, gdt->idGDT);

}


bool  estaEnMetadata(char *nombre_tabla) {

	bool _esLaBuscada(void* tabla){
	return  string_equals_ignore_case((char *) tabla,nombre_tabla);
		}



return	list_any_satisfy(tMetadata->tablas,(void*)estaEnMetadata);



}



