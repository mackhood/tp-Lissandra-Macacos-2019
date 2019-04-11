#include "config_memoria.h"

void levantar_config(){

	conf_mapeada = config_create("memoria.config");

	printf("llegue hasta aca\n");
	info_memoria.puerto = config_get_int_value(conf_mapeada, "PUERTO");
	printf("el puerto es %d\n", info_memoria.puerto);
	//info_memoria.ip_fs = (char*)malloc(IP_SIZE);
	//printf("hice el malloc\n");
	//strcpy(info_memoria.ip_fs, config_get_string_value(conf_mapeada, "IP_FS"));
	//printf("la ip del fs es %s\n", info_memoria.ip_fs);
	printf("llegue hasta aca 2\n");
	info_memoria.puerto_fs = config_get_int_value(conf_mapeada, "PUERTO_FS");
	printf("el puerto del fs es %d\n", info_memoria.puerto_fs);
	//IP_SEED
	//PUERTO SEEDS
	info_memoria.retardo_mp = config_get_int_value(conf_mapeada, "RETARDO_MEM");
	printf("el retardo de mp es %d\n", info_memoria.retardo_mp);
	info_memoria.retardo_fs = config_get_int_value(conf_mapeada, "RETARDO_FS");
	printf("el retardo del fs es %d\n", info_memoria.retardo_fs);
	info_memoria.tamanio_mem = config_get_int_value(conf_mapeada, "TAM_MEM");
	printf("el tamanio de la mem es %d\n", info_memoria.tamanio_mem);
	info_memoria.tiempo_jour = config_get_int_value(conf_mapeada, "RETARDO_JOURNAL");
	printf("el tiempo de jour es %d\n", info_memoria.tiempo_jour);
	info_memoria.tiempo_goss = config_get_int_value(conf_mapeada, "RETARDO_GOSSIPING");
	info_memoria.numero_memoria = config_get_int_value(conf_mapeada, "MEMORY_NUMBER");


	config_destroy(conf_mapeada);

}

void initConfiguracion(){



	t_config* config = config_create("memoria.config");
	if(config  == NULL){
		perror("Error al abrir el archivo de configuracion");
		free(config);
		exit(1);
	}

	/*--------------- cargo todos los datos de kernel.configs -------------------------*/
	memoria_config* memoriaConfig= malloc(sizeof(memoria_config));

	memoriaConfig->puerto =config_get_int_value(config,"PUERTO");
	printf("%d\n", memoriaConfig->puerto);
	memoriaConfig->puerto_fs = config_get_int_value(config,"PUERTO_FS");
	memoriaConfig->retardo_mp = config_get_int_value(config,"RETARDO_MEM");
	memoriaConfig->retardo_fs = config_get_int_value(config,"RETARDO_FS");
	memoriaConfig->tamanio_mem= config_get_int_value(config,"TAM_MEM");
	memoriaConfig->tiempo_jour= config_get_int_value(config,"RETARDO_JOURNAL");
	memoriaConfig->tiempo_goss= config_get_int_value(config,"RETARDO_GOSSIPING");
	memoriaConfig->numero_memoria= config_get_int_value(config,"MEMORY_NUMBER​");

}




