#ifndef LISSANDRAFILESISTEM_H_
#define LISSANDRAFILESISTEM_H_

#include "LFLExternals.h"
#include "Lissandra.h"
#include "Compactador.h"
#include "FileSistem.h"
#include "Consola.h"

/* ESTRUCTURAS */

t_list * hilosLFL;



/* SEMAFOROS */

/* FUNCIONES */
int main(void);
void iniciar();
void setearValores(t_config * archivoConfig);
void iniciarConsola();
void iniciarLissandra();
void iniciarFileSistem();
void iniciarCompactador();
void terminationProtocol();


#endif /* LISSANDRAFILESISTEM_H_ */
