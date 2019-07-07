#include "conexion_memorias.h"

void AceptarMemoria()
{
	//Acepto a la memoria
	struct sockaddr_in direccion_cliente;
	unsigned int tamanio_direccion = sizeof(direccion_cliente);

	socket_memoria = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion);
	printf("Se ha conectado una memoria\n");

	//Creo hilos para atender solicitudes de memoria
	while(  (socket_memoria = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion)) > 0)
		{
			puts("Se ha conectado a una memoria");
			pthread_t RecibirMensajesMemoria;
			pthread_create(&RecibirMensajesMemoria,NULL, (void*)escucharMemoria, NULL);
		}
}
void escucharMemoria(int* memoria){

	int s_memoria = *memoria;
	free(memoria);
	bool connected = true;

	while(connected){
		t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(s_memoria);

		switch(mensaje_recibido->head){

		case SOLICITUD_GOSSIP:
		{
			int tamanio_tabla = list_size(tabla_gossip);
			int tamanio_buffer = 0;

			for (int i=0; i<tamanio_tabla; i++)
			{
				t_est_memoria* memoria = list_get(tabla_gossip, i);
				int tamanio_memoria = sizeof(memoria->numero_memoria) + strlen(memoria->ip_memoria)+sizeof(memoria->puerto_memoria)+3;
				tamanio_buffer += tamanio_memoria;
			}

			char* buffer = malloc(tamanio_buffer+1);
			bool primeraLectura = true;

			for (int i=0; i<tamanio_tabla; i++)
			{
				t_est_memoria* memoria= list_get(tabla_gossip, i);

				if(primeraLectura)
				{
					strcpy(buffer, memoria->numero_memoria);
					primeraLectura = false;
				}
				else
				{
					strcat(buffer, memoria->numero_memoria);
				}

				strcat(buffer, ",");
				strcat(buffer, memoria->ip_memoria);
				strcat(buffer, ",");
				char* puerto = string_itoa(memoria->puerto_memoria);
				strcat(buffer, puerto);
				strcat(buffer, ";");
			}

			int tamanio_buffer_memoria = sizeof(int) + tamanio_buffer;

			void* buffer_memoria = malloc(tamanio_buffer_memoria);

			memcpy(buffer_memoria, &tamanio_buffer, sizeof(int));
			memcpy(buffer_memoria+sizeof(int), buffer, tamanio_buffer);
			prot_enviar_mensaje(socket_memoria, SOLICITUD_GOSSIP, tamanio_buffer_memoria, buffer_memoria);


//				logInfo("Llegó una solicitud de tabla gossip, procedo a enviar memoria por memoria");
//
//				int tamanio_gossip = list_size(tabla_gossip);
//
//				for (int i=0; i<tamanio_gossip; i++){
//
//					t_est_memoria* memoria_enviar = malloc(sizeof(t_est_memoria));
//
//					//envio numero, ip y puerto de cada memoria
//					int numero_enviar = memoria_enviar->numero_memoria;
//					char* ip_enviar = memoria_enviar->ip_memoria;
//					int puerto_enviar = memoria_enviar->puerto_memoria;
//
//					int largo_ip = sizeof(ip_enviar);
//
//					size_t tamanio_buffer = sizeof(int) + largo_ip + sizeof(int);
//					void* buffer = malloc(tamanio_buffer);
//
//					memcpy(buffer, &numero_enviar, sizeof(int));
//					memcpy(buffer+sizeof(int), ip_enviar, IP_SIZE);
//					memcpy(buffer+sizeof(int)+largo_ip, &puerto_enviar, sizeof(int));
//
//					prot_enviar_mensaje(s_memoria, MEMORIA_GOSSIP, tamanio_buffer, buffer);
//
//				}

			break;
		}

		case DESCONEXION:
		{
			puts("Se ha desconectado una memoria");
			connected = false;
			break;
		}

		default:
		{
			break;
		}




	}
	}
}
