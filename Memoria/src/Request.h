#ifndef REQUEST_H_
#define REQUEST_H_

#include "estructurasMemoria.h"

char* selectReq (char* nombre_tabla, uint16_t key);
bool insertReq (char* nombre_tabla, uint16_t key, char* value);
t_prot_mensaje* createReq (char* nombre_tabla, int largo_nombre_tabla, char* tipo_consistencia, int largo_tipo_consistencia, int numero_particiones, int compaction_time);
t_prot_mensaje* dropReq (char* nombre_tabla);
//void journalReq (); --> lo pase a estructuras.h porque lo usamos dentro de la implementacion del LRU

#endif /* REQUEST_H_ */
