#include "Memoria.h"

int main() {

	time_t ahora = time(NULL);
	printf("%ld\n", ahora);

	//nos lo pasa el file system el tamanio del value, pero por ahora es 4 por el ejemplo
	//la key es un uint16_t, tiene 16 bits por ende 2 bytes y no abarca numeros negativos (un int 4 bytes por ende 32 bits y si abarca negativos)
	int tamanio_value = 4;
	//el time_t es en segundos

	int tamanio_pag = sizeof(uint16_t) + tamanio_value + sizeof(time_t);

	levantar_config();
	int cant_marcos = info_memoria.tamanio_mem / tamanio_pag;
	printf("el sizeof int es de %d, el sizeof uint es de %d, el tamanio del time_t es %d, tamanio pag %d, cantidad de marcos %d\n",sizeof(int),sizeof(uint16_t),sizeof(time_t),tamanio_pag, cant_marcos);
	initThread();

	//NO BORRAR DEBIDO A QUE LO USAMOS EN BREVE
	/*socket_kernel = levantar_servidor(info_memoria.puerto);
	t_prot_mensaje* handshake_kernel = prot_recibir_mensaje(socket_kernel);
	printf("el mensaje es %s\n", (char*)handshake_kernel->payload);

	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");*/

	while(1);
	return EXIT_SUCCESS;
}


//________________________________FUNCIONES DE INICIALIZACION DE HILOS Y DEMAS________________________________________________//

void setearValores(){
	return;
}

void initThread(){

	pthread_create(&threadConsola, NULL, (void*)handleConsola, NULL);

	pthread_join(threadConsola ,NULL);
}


