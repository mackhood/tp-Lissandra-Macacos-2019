#include "config_memoria.h"

//void levantar_config(){
//
//
//	t_info * info_memoria = malloc(sizeof(info_memoria));
//	t_config*	conf_mapeada = config_create("/home/utnso/workspace/tp-2019-1c-Macacos/Memoria/memoria.config");
//	info_memoria->puerto = config_get_int_value(conf_mapeada, "PUERTO");
//	info_memoria->ip_fs = malloc(IP_SIZE);
//	strcpy(info_memoria->ip_fs, config_get_string_value(conf_mapeada, "IP_FS"));
//	info_memoria->puerto_fs = config_get_int_value(conf_mapeada, "PUERTO_FS");
//	//IP_SEED
//	//PUERTO SEEDS
//	info_memoria->retardo_mp = config_get_int_value(conf_mapeada, "RETARDO_MEM");
//	info_memoria->retardo_fs = config_get_int_value(conf_mapeada, "RETARDO_FS");
//	info_memoria->tamanio_mem = config_get_int_value(conf_mapeada, "TAM_MEM");
//	info_memoria->tiempo_jour = config_get_int_value(conf_mapeada, "RETARDO_JOURNAL");
//	info_memoria->tiempo_goss = config_get_int_value(conf_mapeada, "RETARDO_GOSSIPING");
//	info_memoria->numero_memoria = config_get_int_value(conf_mapeada, "MEMORY_NUMBER");
//
//	config_destroy(conf_mapeada);
//}
//
//






void initConfiguracion(){



	t_config* config = config_create("memoria.properties");
	if(config  == NULL){
		perror("Error al abrir el archivo de configuracion");
		free(config);
		exit(1);
	}

	/**--------------- cargo todos los datos de kernel.configs -------------------------**/
	memoria_config* memoriaConfig= malloc(sizeof(memoria_config));

	memoriaConfig->puerto = config_get_int_value(config,"PUERTO_MEMORIA");
	memoriaConfig-> puerto_fs= config_get_int_value(config,"METADATA_REFRESH");
	memoriaConfig->retardo_fs = config_get_int_value(config,"RETARDO_FS");
	memoriaConfig->retardo_mp= config_get_int_value(config,"RETARDO_MEM​");
	memoriaConfig->tamanio_mem= config_get_int_value(config,"TAM_MEM");
	memoriaConfig->tiempo_goss= config_get_int_value(config,"MULTIPROCESAMIENTO");
	memoriaConfig->tiempo_jour= config_get_int_value(config,"RETARDO_JOURNAL");

	printf("%d\n", memoriaConfig->puerto);
	printf("%d\n", memoriaConfig->numero_memoria);
	printf("%d\n", memoriaConfig->puerto_fs);
	printf("%d\n", memoriaConfig->retardo_fs);
	printf("%d\n", memoriaConfig->tamanio_mem);

//	char* ip_memoria = config_get_string_value(config,"IP_MEMORIA");
//	memoriaConfig->ip_memoria = malloc( string_length(ip_memoria) + 1 );
//	strcpy(memoriaConfig->ip_memoria, ip_memoria);

//
//	char* algoritmo = config_get_string_value(config,"ALGORITMO_PLANIFICACION");
//	safaConfig->algoritmoPlanificacion = malloc( string_length(algoritmo) + 1 );
//	strcpy(safaConfig->algoritmoPlanificacion, algoritmo);
//
//	safaConfig->gradoMultiprogramacion = config_get_int_value(config,"GRADO_MULTIPRGRAMACION");
//	safaConfig->retardoPlanificacion = config_get_int_value(config,"RETARDO_DE_PLANIFICACION");
	config_destroy(config);


	//logueo todos los datos de configuracion


	t_Memoria = malloc(sizeof(t_memoria));
	t_Memoria->config = memoriaConfig;



}




