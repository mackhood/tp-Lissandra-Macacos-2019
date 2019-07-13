#include "conexion_memorias.h"

void escucharMemoria(int memoria){

	//Comienzo a recibir peticiones
	int s_memoria = memoria;

	t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(s_memoria);

		switch(mensaje_recibido->head){

		case SOLICITUD_GOSSIP:

		{
			logInfo("[ESCUCHAR/MEMORIA]: Se recibió solicitud de tabla gossip por parte de otra memoria");

			int tamanio_tabla = list_size(tabla_gossip);
			int tamanio_buffer = 0;

			for (int i=0; i<tamanio_tabla; i++)
			{
				pthread_mutex_lock(&mutex_tabla_gossiping);
				t_est_memoria* memoria = list_get(tabla_gossip, i);
				pthread_mutex_unlock(&mutex_tabla_gossiping);

				int tamanio_memoria = sizeof(memoria->numero_memoria) + strlen(memoria->ip_memoria)+sizeof(memoria->puerto_memoria)+3;
				tamanio_buffer += tamanio_memoria;
			}

			char* buffer = malloc(tamanio_buffer+1);
			bool primeraLectura = true;

			for (int i=0; i<tamanio_tabla; i++)
			{
				pthread_mutex_lock(&mutex_tabla_gossiping);
				t_est_memoria* memoria= list_get(tabla_gossip, i);
				pthread_mutex_unlock(&mutex_tabla_gossiping);

				if(primeraLectura)
				{
					strcpy(buffer, string_itoa(memoria->numero_memoria));
					primeraLectura = false;
				}
				else
				{
					strcat(buffer, string_itoa(memoria->numero_memoria));
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
			prot_enviar_mensaje(s_memoria, SOLICITUD_GOSSIP, tamanio_buffer_memoria, buffer_memoria);
			logInfo("[ESCUCHAR/MEMORIA]: Se envió tabla gossip a otra memoria");
			free(buffer_memoria);
			break;
		}

		case DESCONEXION:
		{
			puts("Se ha desconectado una memoria");
			logInfo("[ESCUCHAR/MEMORIA]: Se desconectó una memoria");
			break;
		}

		default:
		{
			break;
		}
	}

	close(s_memoria);
	prot_destruir_mensaje(mensaje_recibido);
	logInfo("[ESCUCHAR/MEMORIA]: Se desconectó una memoria");
	printf("Se ha desconectado una memoria\n\n");
}
