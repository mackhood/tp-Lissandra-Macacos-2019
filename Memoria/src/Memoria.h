#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "../../SharedLibrary/protocolo.h"
#include "../../SharedLibrary/conexiones.h"
#include "config_memoria.h"
#include "Consola.h"

int socket_kernel;
int socket_fs;

void setearValores();
void initThread();


pthread_t threadConsola;


#endif /* MEMORIA_H_ */




