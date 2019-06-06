#ifndef CONSOLA_H_
#define CONSOLA_H_

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
void details(char** args);
void show_menu();
void modifyDumpTime(char** args);
void modifyRetardo(char** args);

#endif /* CONSOLA_H_ */
