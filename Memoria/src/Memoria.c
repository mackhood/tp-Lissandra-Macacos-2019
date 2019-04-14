#include "Memoria.h"

int main() {

	levantar_config();
	printf("%d\n", info_memoria.puerto);
	printf("%s\n", info_memoria.ip_fs);
	printf("%d\n", info_memoria.puerto_fs);

	initThread();
	//NO BORRAR DEBIDO A QUE LO USAMOS EN BREVE
	/*socket_kernel = levantar_servidor(info_memoria.puerto);
	t_prot_mensaje* handshake_kernel = prot_recibir_mensaje(socket_kernel);
	printf("el mensaje es %s\n", (char*)handshake_kernel->payload);

	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");*/



	while(1);
	return EXIT_SUCCESS;
}


//_____________________________________________________________________________________________________//



void setearValores(){
	return;
}

void initThread(){

	//logInfo("Creando thread para atender las conexiones de memoria");
	//pthread_create(&threadConexionMemoria, NULL, (void*) handler_conexion_dam_cpu, tSafa);
	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);
	//pthread_create(&threadPlanificador, NULL, (void*)handlePlanificadorLP, tKernel);


	pthread_join(threadConsola ,NULL);
	//pthread_join(threadPlanificador ,NULL);
	//pthread_join(threadConexionMemoria, NULL);

}
