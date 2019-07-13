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
					printf("Se ha conectado el kernel\n");
					escucharYatenderKernel(&cliente);
				}
				else if(cliente == MEMORIA){
					printf("Se ha conectado una memoria\n");
					escucharMemoria(&cliente);
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
