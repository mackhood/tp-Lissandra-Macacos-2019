#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include <dirent.h>
#include "LFLExternals.h"
#include "FileSistem.h"

/* VARIABLES GLOBALES*/

int tiempoDump;
int slowestDump;
int deathProtocol;


/* FUNCIONES */

void mainCompactador();
void setearValoresCompactador(t_config* archivoConfig);
void gestionarTabla(char*tabla);
void compactarTablas(char*tabla);
void gestionarDumps();
void gestionarMemtable();
void crearTemporal(char* tabla);
int perteneceATabla(t_Memtablekeys* key);
void killProtocolCompactador();

#endif /* COMPACTADOR_H_ */
