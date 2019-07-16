#include "Gossiping.h"

void gossiping(){

	tabla_gossip = list_create();
	int numero_gossiping = 1;

	while(1)
	{
		usleep(info_memoria.tiempo_goss * 1000);

		printf("---------- Comienza gossiping! N° %d ---------- \n\n", numero_gossiping);
		printf("Soy la memoria %d, de IP %s y puerto %d \n\n", info_memoria.numero_memoria, info_memoria.ip_memoria, info_memoria.puerto);

		logInfo("[GOSSIPING]: comienza el proceso de gossiping");

		//Primero verifico si me tengo en mi tabla gossip.
		//Si no me tengo, me agrego a mi misma.

		bool yaMeContengo (void* memoria)
		{
			t_est_memoria* memoria_a_comparar = (void*)memoria;

			return memoria_a_comparar->numero_memoria == info_memoria.numero_memoria;
		}

		if (!list_any_satisfy(tabla_gossip, yaMeContengo))
		{
			t_est_memoria* nuevaMemoria = malloc(sizeof(t_est_memoria));

			nuevaMemoria->ip_memoria = info_memoria.ip_memoria;
			nuevaMemoria->puerto_memoria = info_memoria.puerto;
			nuevaMemoria->numero_memoria = info_memoria.numero_memoria;

			pthread_mutex_lock(&mutex_tabla_gossiping);
			list_add(tabla_gossip, nuevaMemoria);
			pthread_mutex_unlock(&mutex_tabla_gossiping);

			printf("Se agregó la memoria número %d a la tabla gossip (me agrego a mi misma) \n", nuevaMemoria->numero_memoria);
			logInfo("[GOSSIPING]: esta memoria se ha agregado a sí misma a la tabla gossip");

		}

		char** ip_seeds = info_memoria.ip_seeds;
		char** puerto_seeds = info_memoria.puerto_seeds;
		int i=0;
		//Comienza gossip: Se descubren y conocen demás memorias

		if (ip_seeds[0]==NULL)
		{
				printf("No existen memorias seed \n");
				logInfo("[GOSSIPING]: No existen memorias seed");
		}

		while (ip_seeds[i]!= NULL)
		{

			//Intento conectarme a la seed.

			char* ip_a_conectar = ip_seeds[i];
			char* puerto = puerto_seeds[i];

			int puerto_a_conectar = atoi(puerto);

			printf("---Intento conectarme con la memoria de ip %s puerto %d \n", ip_a_conectar, puerto_a_conectar);
			logInfo("[GOSSIPING]: Se intentará conectar con memoria seed de ip %s y puerto %d", ip_a_conectar, puerto_a_conectar);

			int conexion = conectar_a_memoria_flexible(ip_a_conectar, puerto_a_conectar, MEMORIA);

			//Consulto si se pudo conectar a la memoria seed

			if (conexion == -3)
			{
					/* Si entro acá, no me pude conectar con la memoria
					 * Verifico si contengo la memoria en la tabla gossip.
					 * Si está, significa que fue desconectada en algún momento, por ende tengo que borrarla de mi tabla
					 */

					bool contenia = false;

					bool conteniaEnTabla (t_est_memoria* memoria)
					{
						if (memoria->puerto_memoria == puerto_a_conectar)
						{
							contenia = true;
							return 1;

						}
						return 0;
					}

					//Acá elimino la memoria de mi tabla gossip, falta saber cómo hacer que se enteren las demas que no se conectan directamente con ella.
					pthread_mutex_lock(&mutex_tabla_gossiping);
					list_remove_and_destroy_by_condition(tabla_gossip, (void*)conteniaEnTabla, &free);
					pthread_mutex_unlock(&mutex_tabla_gossiping);

					if (contenia)
					{
					printf("Se eliminó la memoria de IP %s y puerto %d ya que fue desconectada\n", ip_a_conectar, puerto_a_conectar);
					logInfo("[GOSSIPING]:Se eliminó la memoria de IP %s y puerto %d de la tabla gossip ya que fue desconectada", ip_a_conectar, puerto_a_conectar);
					}
					else
					{
						logInfo("[GOSSIPING]: No se pudo conectar con memoria seed");
					}


			}
			else
			{
				/*	Si fue levantada -> intercambian su tabla gossip agregando nodos faltantes.
					Una memoria conoce las que conoce la otra.
				 */

				printf("Se conectó con otra memoria\n");
				logInfo("[GOSSIPING]: se ha conectado con memoria seed");

				//Solicito tabla de gossip a la memoria seed y le envío la mía.

				//Envío tabla gossip a memoria seed.

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

				prot_enviar_mensaje(conexion, SOLICITUD_GOSSIP, tamanio_buffer_memoria, buffer_memoria);

				free(buffer_memoria);

				t_prot_mensaje* mensaje_con_memorias = prot_recibir_mensaje(conexion);

				//Recibo todas las memorias de la tabla gossip juntas contenidas en un char*
				printf("Recibí mensaje con tabla de gossip \n");
				logInfo("[GOSSIPING]: se recibe tabla gossip de memoria seed");

				int tamanio_mensaje;
				memcpy(&tamanio_mensaje,mensaje_con_memorias->payload,sizeof(int));
				char* mensaje = malloc(tamanio_mensaje + 1);
				memcpy(mensaje, mensaje_con_memorias->payload + sizeof(int), tamanio_mensaje);
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

					printf("La otra memoria conoce a la memoria %d \n", nuevaMemoria->numero_memoria);

					logInfo("[GOSSIPING]: se agregó la memoria %d a la tabla gossip", nuevaMemoria->numero_memoria);

					q++;
				}

				prot_destruir_mensaje(mensaje_con_memorias);

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

			}

			i++;
		}

	printf("\n Actualmente contengo en tabla gossip a: \n");
	int memorias_totales = list_size(tabla_gossip);
	for (int m=0; m<memorias_totales; m++)
	{
		pthread_mutex_lock(&mutex_tabla_gossiping);
		t_est_memoria* memoria_actual = list_get(tabla_gossip, m);
		pthread_mutex_unlock(&mutex_tabla_gossiping);

		printf("Memoria %d \n", memoria_actual->numero_memoria);
	}
	numero_gossiping++;
	}
}
