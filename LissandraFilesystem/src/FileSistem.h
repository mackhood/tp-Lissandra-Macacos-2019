#ifndef FILESISTEM_H_
#define FILESISTEM_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <loggers.h>
#include <dirent.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <commons/collections/list.h>
#include "LFLExternals.h"

/* VARIABLES GLOBALES */

char* punto_montaje;
int tamanio_bloques;
int blocks;
int creatingFL;

/* FUNCIONES */

void mainFileSistem();
void setearValoresFileSistem(t_config * archivoConfig);
void testerFileSystem();
int crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion);
int crearMetadata(char* direccionFinal, char* consistencia, int particiones, int tiempoCompactacion);
int crearParticiones(char* direccionFinal, int particiones);
int dropTable(char* tablaPorEliminar);
int limpiadorDeArchivos(char* direccion);


#endif /* FILESISTEM_H_ */
