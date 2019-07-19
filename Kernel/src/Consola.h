/*
 * Consola.h
 *
 *  Created on: 10 abr. 2019
 *      Author: utnso
 */

#ifndef CONSOLA_H_
#define CONSOLA_H_


#include "stddef.h"
#include "stdlib.h"
#include <readline/readline.h>
#include <commons/string.h>
#include <readline/history.h>
#include <stdio.h>
#include "kernelConfig.h"

#include "InicializacionPcb.h"
#include "adminColasPlanificacion.h"
#include <commons/log.h>
#include <sys/mman.h>

typedef void Funcion ();

typedef struct {
	char* nombre;
	Funcion* funcion;
} COMANDO;


void handleConsola();
COMANDO * buscar_comando(char* linea);
int ejecutar_linea (char * linea);
void selectt (char** args);
void insert (char** args);
void create (char** args);
void describe (char** args);
void drop (char** args);
void journal (char** args);
void add (char** args);
void run (char** args);
void readLatency (char** args) ;
void writeLatency (char** args);
void reads (char** args);
void writes (char** args);
void memoryLoad (char** args);
void metrics (char ** args);

#endif /* CONSOLA_H_ */
