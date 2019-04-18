#ifndef REQUEST_H_
#define REQUEST_H_

#include "estructurasMemoria.h"

void handleConsola();
void selectt (char** args);
void insert (char** args);
void create (char** args);
void describe (char** args);
void drop (char** args);
void journal (char** args);
void add (char** args);
void run (char** args);
void readLatency (char** args) ;
void writeLatency (char** args);
void reads (char** args);
void writes (char** args);
void memoryLoad (char** args);
int ejecutar_linea(char* linea);

#endif /* REQUEST_H_ */
