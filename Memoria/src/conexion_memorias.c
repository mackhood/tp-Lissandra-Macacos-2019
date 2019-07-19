#include "conexion_memorias.h"

void escucharMemoria(int memoria){

	//Comienzo a recibir peticiones
	int s_memoria = memoria;

	t_prot_mensaje* mensaje_recibido = prot_recibir_mensaje(s_memoria);

		switch(mensaje_recibido->head){

		case SOLICITUD_GOSSIP:
		{
			//Acá lo que hago es intercambiar tablas.
			//Primero mando tabla gossip, despues recibo y guardo.

			//Mando tabla
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

			//A partir de acá recibo la tabla
			int tamanio_mensaje;
			memcpy(&tamanio_mensaje,mensaje_recibido->payload,sizeof(int));
			char* mensaje = malloc(tamanio_mensaje + 1);
			memcpy(mensaje, mensaje_recibido->payload + sizeof(int), tamanio_mensaje);
			mensaje[tamanio_mensaje] = '\0';

			//Separo la info de las memorias por ; en datosTAbla

			char ** datosTabla = string_split(mensaje, ";");

			t_list* tablaDeOtraMemoria = list_create();


			//Creo una lista de tablas para guardar memorias directamente
			int q=0;

			//recibo númeroTabla, ip, puerto;

			while(datosTabla[q]!=NULL){

				char* numero_memoria = strtok(datosTabla[q],",");
				char* ip_memoria = strtok(NULL, ",");
				char* puerto_memoria = strtok(NULL, ",");

				t_est_memoria* nuevaMemoria = malloc(sizeof(t_est_memoria));

				int nuevo_numero = atoi(numero_memoria);
				int nuevo_puerto = atoi(puerto_memoria);

				nuevaMemoria->numero_memoria = nuevo_numero;
				nuevaMemoria->ip_memoria = ip_memoria;
				nuevaMemoria->puerto_memoria = nuevo_puerto;


				list_add(tablaDeOtraMemoria, nuevaMemoria);

				logInfo("[GOSSIPING]: se agregó la memoria %d a la tabla gossip", nuevaMemoria->numero_memoria);

				q++;
			}

			int tam = list_size(tablaDeOtraMemoria);

			for (int j=0; j<tam; j++)
			{
				t_est_memoria* memoria_tabla = list_get(tablaDeOtraMemoria, j);

				bool contieneMemoria (void* memoria)
				{
					t_est_memoria* memoria_a_comparar = (void*)memoria;

					return memoria_a_comparar->numero_memoria == memoria_tabla->numero_memoria;
				}

				//verifico si contengo la memoria. Si no está, la agrego a mi tabla gossip.
				pthread_mutex_lock(&mutex_tabla_gossiping);
				bool laContiene = list_any_satisfy(tabla_gossip, contieneMemoria);
				pthread_mutex_unlock(&mutex_tabla_gossiping);

				if(!laContiene)
				{
					pthread_mutex_lock(&mutex_tabla_gossiping);
					list_add(tabla_gossip, memoria_tabla);
					pthread_mutex_unlock(&mutex_tabla_gossiping);

					printf("Se agregó la memoria número %d a mi tabla gossip \n", memoria_tabla->numero_memoria);
					logInfo("[GOSSIPING]: se agregó la memoria %d a la tabla gossip", memoria_tabla->numero_memoria);
				}
			}

		break;
		}

		//DEJO ESTO COMENTADO MIENTRAS TANTO POR LAS DUDAS, IMPLEMENTACIÓN ANTERIOR


//		case SOLICITUD_GOSSIP:
//
//		{
//			logInfo("[ESCUCHAR/MEMORIA]: Se recibió solicitud de tabla gossip por parte de otra memoria");
//
//			int tamanio_tabla = list_size(tabla_gossip);
//			int tamanio_buffer = 0;
//
//			for (int i=0; i<tamanio_tabla; i++)
//			{
//				pthread_mutex_lock(&mutex_tabla_gossiping);
//				t_est_memoria* memoria = list_get(tabla_gossip, i);
//				pthread_mutex_unlock(&mutex_tabla_gossiping);
//
//				int tamanio_memoria = sizeof(memoria->numero_memoria) + strlen(memoria->ip_memoria)+sizeof(memoria->puerto_memoria)+3;
//				tamanio_buffer += tamanio_memoria;
//			}
//
//			char* buffer = malloc(tamanio_buffer+1);
//			bool primeraLectura = true;
//
//			for (int i=0; i<tamanio_tabla; i++)
//			{
//				pthread_mutex_lock(&mutex_tabla_gossiping);
//				t_est_memoria* memoria= list_get(tabla_gossip, i);
//				pthread_mutex_unlock(&mutex_tabla_gossiping);
//
//				if(primeraLectura)
//				{
//					strcpy(buffer, string_itoa(memoria->numero_memoria));
//					primeraLectura = false;
//				}
//				else
//				{
//					strcat(buffer, string_itoa(memoria->numero_memoria));
//				}
//
//				strcat(buffer, ",");
//				strcat(buffer, memoria->ip_memoria);
//				strcat(buffer, ",");
//				char* puerto = string_itoa(memoria->puerto_memoria);
//				strcat(buffer, puerto);
//				strcat(buffer, ";");
//			}
//
//			int tamanio_buffer_memoria = sizeof(int) + tamanio_buffer;
//
//			void* buffer_memoria = malloc(tamanio_buffer_memoria);
//
//			memcpy(buffer_memoria, &tamanio_buffer, sizeof(int));
//			memcpy(buffer_memoria+sizeof(int), buffer, tamanio_buffer);
//			prot_enviar_mensaje(s_memoria, SOLICITUD_GOSSIP, tamanio_buffer_memoria, buffer_memoria);
//			logInfo("[ESCUCHAR/MEMORIA]: Se envió tabla gossip a otra memoria");
//			free(buffer_memoria);
//			break;
//		}
//
//		case TABLA_GOSSIP:
//
//		{
//			int tamanio_mensaje;
//			memcpy(&tamanio_mensaje,mensaje_recibido->payload,sizeof(int));
//			char* mensaje = malloc(tamanio_mensaje + 1);
//			memcpy(mensaje, mensaje_recibido->payload + sizeof(int), tamanio_mensaje);
//			mensaje[tamanio_mensaje] = '\0';
//
//			//Separo la info de las memorias por ; en datosTAbla
//
//			char ** datosTabla = string_split(mensaje, ";");
//
//			t_list* tablaDeOtraMemoria = list_create();
//
//
//			//Creo una lista de tablas para guardar memorias directamente
//			int q=0;
//
//			//recibo númeroTabla, ip, puerto;
//
//			while(datosTabla[q]!=NULL){
//
//				char* numero_memoria = strtok(datosTabla[q],",");
//				char* ip_memoria = strtok(NULL, ",");
//				char* puerto_memoria = strtok(NULL, ",");
//
//				t_est_memoria* nuevaMemoria = malloc(sizeof(t_est_memoria));
//
//				int nuevo_numero = atoi(numero_memoria);
//				int nuevo_puerto = atoi(puerto_memoria);
//
//				nuevaMemoria->numero_memoria = nuevo_numero;
//				nuevaMemoria->ip_memoria = ip_memoria;
//				nuevaMemoria->puerto_memoria = nuevo_puerto;
//
//
//				list_add(tablaDeOtraMemoria, nuevaMemoria);
//
//				printf("La otra memoria conoce a la memoria %d \n", nuevaMemoria->numero_memoria);
//
//				logInfo("[GOSSIPING]: se agregó la memoria %d a la tabla gossip", nuevaMemoria->numero_memoria);
//
//				q++;
//			}
//
//			prot_destruir_mensaje(mensaje_recibido);
//
//			int tam = list_size(tablaDeOtraMemoria);
//
//			for (int j=0; j<tam; j++)
//			{
//				t_est_memoria* memoria_tabla = list_get(tablaDeOtraMemoria, j);
//
//				bool contieneMemoria (void* memoria)
//				{
//					t_est_memoria* memoria_a_comparar = (void*)memoria;
//
//					return memoria_a_comparar->numero_memoria == memoria_tabla->numero_memoria;
//				}
//
//				//verifico si contengo la memoria. Si no está, la agrego a mi tabla gossip.
//				pthread_mutex_lock(&mutex_tabla_gossiping);
//				bool laContiene = list_any_satisfy(tabla_gossip, contieneMemoria);
//				pthread_mutex_unlock(&mutex_tabla_gossiping);
//
//				if(!laContiene)
//				{
//					pthread_mutex_lock(&mutex_tabla_gossiping);
//					list_add(tabla_gossip, memoria_tabla);
//					pthread_mutex_unlock(&mutex_tabla_gossiping);
//
//					printf("Se agregó la memoria número %d a mi tabla gossip \n", memoria_tabla->numero_memoria);
//					logInfo("[GOSSIPING]: se agregó la memoria %d a la tabla gossip", memoria_tabla->numero_memoria);
//				}
//			}
//
//
//			break;
//		}
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
