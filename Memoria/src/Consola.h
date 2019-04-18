#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "estructurasMemoria.h"
#include "Request.h"
//no se que hace esta biblio pero la dejo
#include "stddef.h"

typedef void Funcion ();

typedef struct {
	char* nombre;
	Funcion* funcion;
} COMANDO;

#endif /* CONSOLA_H_ */

