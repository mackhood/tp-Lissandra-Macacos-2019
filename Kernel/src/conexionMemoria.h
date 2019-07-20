/*
 * conexionMemoria.h
 *
 *  Created on: 15 abr. 2019
 *      Author: utnso
 */

#ifndef CONEXIONMEMORIA_H_
#define CONEXIONMEMORIA_H_


#include "../../SharedLibrary/conexiones.h"
#include "kernelConfig.h"

void handler_conexion_memoria(t_kernel*);
memoria* crearMemoria(char* ip,int puerto,int numeroMemoriaGeneral);

#endif /* CONEXIONMEMORIA_H_ */
