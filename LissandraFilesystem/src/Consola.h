#ifndef CONSOLA_H_
#define CONSOLA_H_

#include "stddef.h"
#include "stdlib.h"
#include "Lissandra.h"
#include "LFLExternals.h"

/* ESTRUCTURAS */
typedef void Funcion ();

typedef struct {
	char* nombre;
	Funcion* funcion;
} COMANDO;
/* FUNCIONES */

void consola();
COMANDO * buscar_comando(char* linea);
int ejecutar_linea (char * linea);
void selectt (char** args);
void insert (char** args);
void create (char** args);
void describe (char** args);
void drop (char** args);
int chequearParametros(char** args, int cantParametros);

#endif /* CONSOLA_H_ */
