#ifndef COMPACTADOR_H_
#define COMPACTADOR_H_

#include <dirent.h>
#include "LFLExternals.h"

int tiempoDump;

/* FUNCIONES */

void mainCompactador();
void setearValoresCompactador(t_config* archivoConfig);
void gestionarTabla(char*tabla);
void compactarTablas(char*tabla);

#endif /* COMPACTADOR_H_ */
