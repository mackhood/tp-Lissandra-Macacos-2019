#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "Request.h"
//no se que hace esta biblio pero la dejo
#include "stddef.h"

typedef void Funcion ();

typedef struct {
	char* nombre;
	Funcion* funcion;
} COMANDO;

void handleConsola();
int ejecutar_linea (char * linea);
void selectt(char** args);
void insert(char** args);
void create(char** args);
void drop(char** args);

#endif /* CONSOLA_H_ */

