#ifndef CONFIG_MEMORIA_H_
#define CONFIG_MEMORIA_H_

#include "estructurasMemoria.h"

//Tamaño de la ip
#define IP_SIZE (sizeof(char) * (strlen("123.000.000.001") + 1))

typedef struct{
	int puerto;
	char* ip_fs;
	int puerto_fs;
	//char** ip_seeds;
	// puerto seeds;
	int retardo_mp;
	int retardo_fs;
	int tamanio_mem;
	int tiempo_jour;
	int tiempo_goss;
	int numero_memoria;

}t_info;

t_config *conf_mapeada;
t_info info_memoria;

void levantar_config();

#endif /* CONFIG_MEMORIA_H_ */
