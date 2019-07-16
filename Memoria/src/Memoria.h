#ifndef MEMORIA_H_
#define MEMORIA_H_

#include "Consola.h"
#include "aceptar_clientes.h"
#include "Gossiping.h"
#include "Notify.h"
#include "TimerJournal.h"
#include "config_memoria.h"

int main(int argc, char** argv);

pthread_mutex_t mutex_conexion_memorias;

#endif /* MEMORIA_H_ */




