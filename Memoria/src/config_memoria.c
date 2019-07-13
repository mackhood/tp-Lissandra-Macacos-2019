#include "config_memoria.h"

void levantar_config(){

	conf_mapeada = config_create("/home/utnso/workspace/tp-2019-1c-Macacos/Memoria/memoria.config");


			info_memoria.ip_memoria = (char*)malloc(IP_SIZE);
			strcpy(info_memoria.ip_memoria, config_get_string_value(conf_mapeada, "IP_MEMORIA"));
			info_memoria.puerto = config_get_int_value(conf_mapeada, "PUERTO");

			info_memoria.ip_fs = (char*)malloc(IP_SIZE);
			strcpy(info_memoria.ip_fs, config_get_string_value(conf_mapeada, "IP_FS"));
			info_memoria.puerto_fs = config_get_int_value(conf_mapeada, "PUERTO_FS");

			char* ip_seeds = strdup(config_get_string_value(conf_mapeada, "IP_SEEDS"));
			info_memoria.ip_seeds = string_get_string_as_array(ip_seeds);
			free(ip_seeds);

			char* puerto_seeds = strdup(config_get_string_value(conf_mapeada, "PUERTO_SEEDS"));
			info_memoria.puerto_seeds = string_get_string_as_array(puerto_seeds);
			free(puerto_seeds);

			info_memoria.retardo_mp = config_get_int_value(conf_mapeada, "RETARDO_MEM");
			info_memoria.retardo_fs = config_get_int_value(conf_mapeada, "RETARDO_FS");
			info_memoria.tamanio_mem = config_get_int_value(conf_mapeada, "TAM_MEM");
			info_memoria.tiempo_jour = config_get_int_value(conf_mapeada, "RETARDO_JOURNAL");
			info_memoria.tiempo_goss = config_get_int_value(conf_mapeada, "RETARDO_GOSSIPING");
			info_memoria.numero_memoria = config_get_int_value(conf_mapeada, "MEMORY_NUMBER");

	config_destroy(conf_mapeada);
}



