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
#include <commons/collections/list.h>



/* FUNCIONES */

void mainFileSistem();
void crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion);

#endif /* FILESISTEM_H_ */
