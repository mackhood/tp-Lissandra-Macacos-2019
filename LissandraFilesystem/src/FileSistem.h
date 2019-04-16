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

/* FUNCIONES */

void mainFileSistem();
void setearValoresFileSistem(t_config * archivoConfig);
void crearTabla(char* nombre, char* consistencia, char* particiones, char* tiempoCompactacion);
int crearMetadata(char* direccionFinal, char* consistencia, char* particiones, char* tiempoCompactacion);
int crearParticiones(char* direccionFinal);

#endif /* FILESISTEM_H_ */
