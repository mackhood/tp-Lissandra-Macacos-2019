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

int tamanio_bloques;
int blocks;
int creatingFL;

/* FUNCIONES */

void mainFileSistem();
void setearValoresFileSistem(t_config * archivoConfig);
void testerFileSystem();
void levantarBitmap(char* direccion);
int crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion);
int crearMetadata(char* direccionFinal, char* consistencia, int particiones, int tiempoCompactacion);
int crearParticiones(char* direccionFinal, int particiones);
int dropTable(char* tablaPorEliminar);
int limpiadorDeArchivos(char* direccion);
int existeTabla(char* tabla);
int mostrarMetadataEspecificada(char* tabla, int tamanio_buffer_metadatas, bool solicitadoPorMemoria, char* buffer);
void mostrarTodosLosMetadatas(bool solicitadoPorMemoria, char* buffer);
int contarTablasExistentes();


#endif /* FILESISTEM_H_ */
