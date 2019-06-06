#ifndef REQUEST_H_
#define REQUEST_H_

#include "estructurasMemoria.h"

char* selectReq (char* nombre_tabla, uint16_t key);
double insertReq (char* nombre_tabla, uint16_t key, char* value);
t_prot_mensaje* createReq (char* nombre_tabla, int largo_nombre_tabla, char* tipo_consistencia, int largo_tipo_consistencia, int numero_particiones, int compaction_time);
void describe (char* nombre_tabla);
void dropReq (char* nombre_tabla);
//void journal (); --> lo pase a estructuras.h porque lo usamos dentro de la implementacion del LRU
bool _esPaginaModificada(void* pagina);

#endif /* REQUEST_H_ */
