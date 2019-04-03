#ifndef LISSANDRA_H_
#define LISSANDRA_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/log.h>
#include <pthread.h>
#include <loggers.h>
#include <string.h>



int retardo;
int puerto_de_escucha;
int tiempo_dump;
int punto_de_montaje;

void setearValores(t_config * archivoConfig);





#endif /* LISSANDRA_H_ */
