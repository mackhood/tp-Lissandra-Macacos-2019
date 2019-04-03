#ifndef LISSANDRAFILESISTEM_H_
#define LISSANDRAFILESISTEM_H_

#include "Lissandra.h"
#include "Compactador.h"
#include "FileSistem.h"
#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <loggers.h>
#include <string.h>

/* VARIABLES GLOBALES */
t_log* loggerLFL;

void setearValores(t_config * archivoConfig);
int main(void);


#endif /* LISSANDRAFILESISTEM_H_ */
