#include "aceptar_clientes.h"

void aceptarClientes(){

	//Acepto al Kernel
	struct sockaddr_in direccion_cliente;
	unsigned int tamanio_direccion = sizeof(direccion_cliente);
	//pthread_mutex_t mutex_del_while;
	//pthread_mutex_init(&mutex_del_while, NULL);

	int connected = 1;
	//Creo hilos para atender solicitudes del Kernel
	while(connected)
		{
			//pthread_mutex_lock(&mutex_del_while);
			socket_cliente = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion);
			if(socket_cliente > 0){

				t_prot_mensaje* mensaje_inicial_conexion = prot_recibir_mensaje(socket_cliente);
				t_cliente cliente;/* = *(t_cliente*)(mensaje_inicial_conexion->payload);*/
				memcpy(&cliente, mensaje_inicial_conexion->payload, sizeof(t_cliente));
				if(cliente == KERNEL){
					int* kernel = (int*) malloc (sizeof(int));
					*kernel = socket_cliente;
					printf("Se ha conectado el kernel\n");
					pthread_t RecibirMensajesKernel;
					pthread_create(&RecibirMensajesKernel,NULL, (void*)escucharYatenderKernel, kernel);
					pthread_join(RecibirMensajesKernel, NULL);
				}
				else if(cliente == MEMORIA){
					int* memoria = (int*) malloc (sizeof(int));
					*memoria = socket_cliente;
					printf("Se ha conectado una memoria\n");
					pthread_t RecibirMensajesMemoria;
					pthread_create(&RecibirMensajesMemoria, NULL, (void*)escucharMemoria, memoria);
					pthread_join(RecibirMensajesMemoria, NULL);
				}
			}
			else
			{
				printf("Fallo el accept\n");
				connected = 0;
			}
			//pthread_mutex_unlock(&mutex_del_while);
		}
}
