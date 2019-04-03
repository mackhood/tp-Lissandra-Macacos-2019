#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <loggers.h>
#include <string.h>

/* VARIABLES GLOBALES */
t_log* loggerLFL;

int retardo;
int puerto_de_escucha;
int tiempo_dump;

void setearValores(t_config * archivoConfig);





#endif /* LISSANDRA_H_ */
