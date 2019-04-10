#include "config_memoria.h"

void levantar_config(){

	conf_mapeada = config_create("memoria.properties");

	printf("llegue hasta aca\n");
	info_memoria.puerto = config_get_int_value(conf_mapeada, "PUERTO");
	printf("el puerto es %d\n", info_memoria.puerto);
	info_memoria.ip_fs = malloc(IP_SIZE);
	strcpy(info_memoria.ip_fs, config_get_string_value(conf_mapeada, "IP_FS"));
	info_memoria.puerto_fs = config_get_int_value(conf_mapeada, "PUERTO_FS");
	//IP_SEED
	//PUERTO SEEDS
	info_memoria.retardo_mp = config_get_int_value(conf_mapeada, "RETARDO_MEM");
	info_memoria.retardo_fs = config_get_int_value(conf_mapeada, "RETARDO_FS");
	info_memoria.tamanio_mem = config_get_int_value(conf_mapeada, "TAM_MEM");
	info_memoria.tiempo_jour = config_get_int_value(conf_mapeada, "RETARDO_JOURNAL");
	info_memoria.tiempo_goss = config_get_int_value(conf_mapeada, "RETARDO_GOSSIPING");
	info_memoria.numero_memoria = config_get_int_value(conf_mapeada, "MEMORY_NUMBER");

	config_destroy(conf_mapeada);
}




