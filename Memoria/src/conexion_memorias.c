#include "conexion_memorias.h"

void escucharMemoria(int* memoria){
	int s_memoria = *memoria;
	free(memoria);

	while(1){

		t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(s_memoria);

		switch(mensaje_recibido->head){

		case SOLICITUD_GOSSIP:
		{

				logInfo("Llegó una solicitud de tabla gossip, procedo a enviar memoria por memoria");

				int tamanio_gossip = list_size(tabla_gossip);

				for (int i=0; i<tamanio_gossip; i++){

					t_est_memoria* memoria_enviar = malloc(sizeof(t_est_memoria));

					//envio numero, ip y puerto de cada memoria
					int numero_enviar = memoria_enviar->numero_memoria;
					char* ip_enviar = memoria_enviar->ip_memoria;
					int puerto_enviar = memoria_enviar->puerto_memoria;

					int largo_ip = sizeof(ip_enviar);

					size_t tamanio_buffer = sizeof(int) + largo_ip + sizeof(int);
					void* buffer = malloc(tamanio_buffer);

					memcpy(buffer, &numero_enviar, sizeof(int));
					memcpy(buffer+sizeof(int), ip_enviar, IP_SIZE);
					memcpy(buffer+sizeof(int)+largo_ip, &puerto_enviar, sizeof(int));

					prot_enviar_mensaje(s_memoria, MEMORIA_GOSSIP, tamanio_buffer, buffer);

				}

			break;
		}

		default:
		{
			break;
		}



		}
	}
}
