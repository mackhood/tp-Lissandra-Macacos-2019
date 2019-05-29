#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include <dirent.h>
#include "LFLExternals.h"
#include "FileSistem.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

/* VARIABLES GLOBALES*/

int tiempoDump;
int slowestCompactationInterval;
int lastDumpSituation;
char* direccionDeLasTablas;
struct dirent* TableEntry;
DIR* directorioDeTablas;

/* FUNCIONES */

void mainCompactador();
void setearValoresCompactador(t_config* archivoConfig);
void gestionarTabla(char*tabla);
void compactarTablas(char*tabla);
void gestionarDumps();
void gestionarMemtable();
void crearTemporal(char* tabla);
int perteneceATabla(t_Memtablekeys* key);
void ejecutarCompactacion(char* tabla);
char* obtenerKeysAPlasmar(t_list* keysPostParsing, int numeroDeParticion, int particiones);
void killProtocolCompactador();

#endif /* COMPACTADOR_H_ */
