#include "Lissandra.h"

void inicializar()
{
	hilos = list_create();
	memorias = list_create();
	compactadores = list_create();
	memtable = list_create();
	killthreads = false;
	iniciarServidor();
}

void setearValoresLissandra(t_config * archivoConfig)
{
	retardo = config_get_int_value(archivoConfig, "RETARDO");
	server_puerto = config_get_int_value(archivoConfig, "PUERTO_ESCUCHA");
	server_ip = strdup(config_get_string_value(archivoConfig,"IP_FILE_SYSTEM"));
	tamanio_value = config_get_int_value(archivoConfig, "TAMANIO_VALUE");
}

void mainLissandra ()
{
	inicializar();
}

void iniciarServidor()
{
	socket_escucha = levantar_servidor(server_puerto);

	int socket_memoria;
	struct sockaddr_in direccion_cliente;
	unsigned int tamanio_direccion = sizeof(direccion_cliente);

	//Se aceptan clientes cuando los haya
	// accept es una funcion bloqueante, si no hay ningun cliente esperando ser atendido, se queda esperando a que venga uno.
	while(  (socket_memoria = accept(socket_escucha, (void*) &direccion_cliente, &tamanio_direccion)) > 0){
		//la memoria nos enviara un mensaje inicial que diga quien es
		t_prot_mensaje* mensaje = prot_recibir_mensaje(socket_memoria);

		uint16_t key_recibida;
		double hora_actual;
		int tamanio_value;

		memcpy(&key_recibida, mensaje->payload, sizeof(uint16_t));
		memcpy(&hora_actual, mensaje->payload + sizeof(uint16_t), sizeof(double));
		memcpy(&tamanio_value, mensaje->payload + sizeof(uint16_t) + sizeof(double), sizeof(int));

		char* value = malloc(tamanio_value + 1);
		memcpy(value, mensaje->payload + sizeof(uint16_t) + sizeof(double) + sizeof(int), tamanio_value);
		value[tamanio_value] = '\0';

		printf("el CLIENTE es %s y nos manda de prueba la key %d y la hora %lf\n\n", value, key_recibida, hora_actual);

		prot_destruir_mensaje(mensaje);
		logInfo( "[Lissandra]: Se conecto una Memoria");
		pthread_t RecibirMensajesMemoria;
		/*Duplico la variable que tiene el valor del socket del cliente*/
		int* memoria = (int*) malloc (sizeof(int));
		*memoria = socket_memoria;
		pthread_create(&RecibirMensajesMemoria,NULL, (void*)escucharMemoria, memoria);

		}
}

void escucharMemoria(int* socket_memoria)
{
	int socket = *socket_memoria;
	bool memoriaDesconectada = false;
	while(!killthreads)
	{
		t_prot_mensaje* mensaje_memoria = prot_recibir_mensaje(socket);

		switch(mensaje_memoria->head)
		{
			case HANDSHAKE:
			{
				logInfo( "Lissandra: me llegó un handshake de una Memoria, procedo a enviar los datos necesarios");
//				bool solicitadoPorMemoria = true;
//				char* buffer = string_new();
//				if(0 == describirTablas("", solicitadoPorMemoria, buffer))
//				{
//					size_t tamanioBuffer = strlen(buffer);
//					void* messageBuffer = malloc(tamanioBuffer + 1);
//					memcpy(messageBuffer, buffer, strlen(buffer));
//					prot_enviar_mensaje(socket, FULL_DESCRIBE, tamanioBuffer, messageBuffer);
//					logInfo("Lissandra: Se ha enviado la metadata de todas las tablas a Memoria.");
//				}
//				else
//				{
//					prot_enviar_mensaje(socket, FAILED_DESCRIBE, 0, NULL);
//					logError("Lissandra: falló al leer todas las tablas");
//				}
//				free(buffer);

				size_t tamanioBuffer = sizeof(int);
				void* messageBuffer = malloc(tamanioBuffer + 1);
				memcpy(messageBuffer, &tamanio_value, sizeof(int));
				prot_enviar_mensaje(socket, HANDSHAKE, tamanioBuffer, messageBuffer);
				logInfo("Lissandra: Se ha enviado la información de saludo a la Memoria.");
				break;
			}
			case SOLICITUD_TABLA:
			{
				logInfo( "Lissandra: Nos llega un pedido de Select de parte de la Memoria");
				uint16_t auxkey;
				char* tabla;
				int tamanioNombre;
				memcpy(&auxkey, mensaje_memoria->payload, sizeof(uint16_t));
				memcpy(&tamanioNombre, mensaje_memoria->payload + sizeof(uint16_t), sizeof(int));
				tabla = malloc(tamanioNombre + 1);
				memcpy(tabla, mensaje_memoria->payload + sizeof(uint16_t) + sizeof(int), tamanioNombre);
				tabla[tamanioNombre] = '\0';

//				double tiempo_pag = getCurrentTime();
//				char* value = "Ejemplo";
//				int tamanio_value = strlen(value);
//
//				size_t tamanio_buffer = (sizeof(double)+tamanio_value+sizeof(int));
//				void* buffer = malloc(tamanio_buffer);
//
//				memcpy(buffer, &tiempo_pag, sizeof(double));
//				memcpy(buffer+sizeof(double), &tamanio_value, sizeof(int));
//				memcpy(buffer+sizeof(double)+sizeof(int), value, tamanio_value);
//
//				prot_enviar_mensaje(socket, VALUE_SOLICITADO_OK, tamanio_buffer, buffer);

				 t_keysetter* helpinghand = selectKey(tabla, auxkey);
				 if(helpinghand != NULL)
				 {
					double tiempo_pag = helpinghand->timestamp;
					char* value = helpinghand->clave;
					int tamanio_value = strlen(value);
					size_t tamanio_buffer = (sizeof(double)+tamanio_value+sizeof(int));
					void* buffer = malloc(tamanio_buffer);

					memcpy(buffer, &tiempo_pag, sizeof(double));
					memcpy(buffer+sizeof(double), &tamanio_value, sizeof(int));
					memcpy(buffer+sizeof(double)+sizeof(int), value, tamanio_value);

					prot_enviar_mensaje(socket, VALUE_SOLICITADO_OK, tamanio_buffer, buffer);
				 }
				 else
				 {
				 	 prot_enviar_mensaje(socket, VALUE_FAILURE, 0, NULL);
				 }

				break;
			}
			case CREATE_TABLA:
			{
				logInfo( "Lissandra: Llega un pedido de Create de parte de Memoria");
				char* tablaRecibida = string_new();
				char* consistenciaRecibida = string_new();
				int cantParticionesRecibida;
				int tiempoEntreCompactacionesRecibido;
				int tamanioNombreTabla;
				int tamanioConsistencia;
				memcpy(&tamanioNombreTabla, mensaje_memoria->payload, sizeof(int));
				tablaRecibida = malloc(tamanioNombreTabla + 2);
				memcpy(tablaRecibida, mensaje_memoria->payload + sizeof(int), tamanioNombreTabla);
				tablaRecibida[tamanioNombreTabla] = '\0';
				memcpy(&tamanioConsistencia, mensaje_memoria->payload + sizeof(int) + tamanioNombreTabla, sizeof(int));
				consistenciaRecibida = malloc(tamanioConsistencia + 2);
				memcpy(consistenciaRecibida, mensaje_memoria->payload + sizeof(int) + tamanioNombreTabla +sizeof(int),
						tamanioConsistencia);
				consistenciaRecibida[tamanioConsistencia] = '\0';
				memcpy(&cantParticionesRecibida, mensaje_memoria->payload + sizeof(int) + tamanioNombreTabla +sizeof(int)
						+ tamanioConsistencia, sizeof(int));
				memcpy(&tiempoEntreCompactacionesRecibido, mensaje_memoria->payload + sizeof(int) + tamanioNombreTabla
						+ sizeof(int) + tamanioConsistencia + sizeof(int), sizeof(int));
				switch(llamadoACrearTabla(tablaRecibida, consistenciaRecibida, cantParticionesRecibida, tiempoEntreCompactacionesRecibido))
				{
					case 0:
					{
						logInfo( "Lissandra: Tabla creada satisfactoriamente a pedido de Memoria");
						prot_enviar_mensaje(socket, TABLA_CREADA_OK, 0, NULL);
						break;
					}
					case 2:
					{
						logInfo( "Lissandra: Tabla ya existía, lamentablemente para Memoria");
						prot_enviar_mensaje(socket, TABLA_CREADA_YA_EXISTENTE, 0, NULL);
						break;
					}
					default:
					{
						logError( "Lissandra: La tabla o alguna de sus partes no pudo ser creada, informo a Memoria");
						prot_enviar_mensaje(socket, TABLA_CREADA_FALLO, 0, NULL);
						break;
					}
				}
				break;
			}
			case TABLE_DROP:
			{
				char* tablaRecibida = string_new();
				int tamanioNombreTabla;
				memcpy(&tamanioNombreTabla, mensaje_memoria->payload, sizeof(int));
				tablaRecibida = malloc(tamanioNombreTabla + 2);
				memcpy(tablaRecibida, mensaje_memoria->payload + sizeof(int), tamanioNombreTabla);
				tablaRecibida[tamanioNombreTabla] = '\0';
				int results = llamarEliminarTabla(tablaRecibida);
				switch(results)
				{
					case 1:
					{
						prot_enviar_mensaje(socket, TABLE_DROP_NO_EXISTE, 0, NULL);
						logError( "Lissandra: La %s no existe y no se puede eliminar.", tablaRecibida);
						break;
					}
					case 0:
					{
						prot_enviar_mensaje(socket, TABLE_DROP_OK, 0, NULL);
						logInfo( "Lissandra: La %s fue eliminada correctamente", tablaRecibida);
						break;
					}
					default:
					{
						prot_enviar_mensaje(socket, TABLE_DROP_FALLO, 0, NULL);
						logError( "Lissandra: la operacion no fue terminada por un fallo en acceder a la %s", tablaRecibida);
						break;
					}
				}
				break;
			}
			case DESCRIBE:
			{
				bool solicitadoPorMemoria = true;
				if(mensaje_memoria->payload != NULL)
				{
					char* tablaRecibida = string_new();
					int tamanioNombreTabla;
					memcpy(&tamanioNombreTabla, mensaje_memoria->payload, sizeof(int));
					tablaRecibida = malloc(tamanioNombreTabla + 2);
					memcpy(tablaRecibida, mensaje_memoria->payload + sizeof(int), tamanioNombreTabla);
					tablaRecibida[tamanioNombreTabla] = '\0';
					char* buffer = string_new();
					if(0 == describirTablas(tablaRecibida, solicitadoPorMemoria, buffer))
					{
						size_t tamanioBuffer = strlen(buffer);
						void* messageBuffer = malloc(tamanioBuffer + 1);
						memcpy(messageBuffer, buffer, strlen(buffer));
						prot_enviar_mensaje(socket, POINT_DESCRIBE, tamanioBuffer, messageBuffer);
						logInfo( "Lissandra: Se ha enviado la metadata de la %s a Memoria.", tablaRecibida);
					}
					else
					{
						prot_enviar_mensaje(socket, FAILED_DESCRIBE, 0, NULL);
						logError( "Lissandra: fallo al leer la %s", tablaRecibida);
					}
					free(tablaRecibida);
					free(buffer);
				}
				else
				{
					char* buffer = string_new();
					if(0 == describirTablas("", solicitadoPorMemoria, buffer))
					{
						size_t tamanioBuffer = strlen(buffer);
						void* messageBuffer = malloc(tamanioBuffer + 1);
						memcpy(messageBuffer, buffer, strlen(buffer));
						prot_enviar_mensaje(socket, FULL_DESCRIBE, tamanioBuffer, messageBuffer);
						logInfo( "Lissandra: Se ha enviado la metadata de todas las tablas a Memoria.");
					}
					else
					{
						prot_enviar_mensaje(socket, FAILED_DESCRIBE, 0, NULL);
						logError( "Lissandra: falló al leer todas las tablas");
					}
					free(buffer);
				}
				break;
			}
			case JOURNALING_INSERT:
			{
				char* tablaRecibida = string_new();
				char* valueRecibido = string_new();
				uint16_t keyRecibida;
				double timestampRecibido;
				int tamanioNombreTabla;
				int tamanioValue;
				memcpy(&timestampRecibido, mensaje_memoria->payload, sizeof(double));
				memcpy(&tamanioNombreTabla, mensaje_memoria->payload + sizeof(double), sizeof(int));
				tablaRecibida = malloc(tamanioNombreTabla + 2);
				memcpy(tablaRecibida, mensaje_memoria->payload + sizeof(double) + sizeof(int), tamanioNombreTabla);
				tablaRecibida[tamanioNombreTabla] = '\0';
				memcpy(&keyRecibida, mensaje_memoria->payload + sizeof(double) + sizeof(int) + tamanioNombreTabla
						, sizeof(uint16_t));
				memcpy(&tamanioValue, mensaje_memoria->payload + sizeof(double) + sizeof(int) + tamanioNombreTabla + sizeof(uint16_t)
						, sizeof(int));
				valueRecibido = malloc(tamanioValue + 2);
				memcpy(valueRecibido, mensaje_memoria->payload + sizeof(double) + sizeof(int) + tamanioNombreTabla + sizeof(uint16_t)
						+ sizeof(int), tamanioValue);
				valueRecibido[tamanioValue] = '\0';

				switch(insertKeysetter(tablaRecibida, keyRecibida, valueRecibido, timestampRecibido))
				{
					case 0:
					{
						prot_enviar_mensaje(socket, INSERT_SUCCESSFUL, 0, NULL);
						break;
					}
					case 2:
					{
						prot_enviar_mensaje(socket, INSERT_FAILED_ON_MEMTABLE, 0, NULL);
						break;
					}
					default:
					{
						prot_enviar_mensaje(socket, INSERT_FAILURE, 0, NULL);
						break;
					}
				}
				break;
			}
			case DESCONEXION:
			{
				memoriaDesconectada = true;
				break;
			}
			default:
			{
				break;
			}
		}
		if(memoriaDesconectada)
		{
			logInfo("Una memoria se ha desconectado");
			break;
		}
		usleep(retardo * 1000);
	}
	if(!memoriaDesconectada)
	{
		prot_enviar_mensaje(socket, GOODBYE, 0, NULL);
	}
}

int insertKeysetter(char* tablaRecibida, uint16_t keyRecibida, char* valueRecibido, double timestampRecibido)
{
	tamanio_memtable = memtable->elements_count;
	t_Memtablekeys* auxiliar = malloc(sizeof(t_Memtablekeys) + 4);
	t_keysetter* auxiliarprima = malloc(sizeof(t_keysetter) + 3);
	auxiliarprima->key = keyRecibida;
	auxiliarprima->clave = valueRecibido;
	auxiliarprima->timestamp = timestampRecibido;
	auxiliar->tabla = tablaRecibida;
	auxiliar->data = auxiliarprima;

	printf("\033[1;34m");
	printf("%i, %s,", auxiliar->data->key, auxiliar->tabla);
	printf(" %s, %lf\n", auxiliar->data->clave, auxiliar->data->timestamp);
	if(0 == existeTabla(tablaRecibida))
	{
		logError( "Lissandra: La tabla no existe, por lo que no puede insertarse una clave.");
		printf("Tabla no existente.\n");
		return 1;
	}
	else
	{
		if(strlen(valueRecibido) > tamanio_value)
		{
			logError("Lissandra: el value a agregar era demasiado grande");
			printf("El value ingresado era demasiado grande, por favor, ingrese uno más pequeño.\n");
			return 3;
		}
		pthread_mutex_lock(&dumpEnCurso);
		pthread_mutex_unlock(&dumpEnCurso);
		logInfo( "Lissandra: Se procede a insertar la clave recibida en la Memtable.");
		list_add(memtable, auxiliar);
		if(tamanio_memtable == memtable->elements_count)
		{
			logError( "Lissandra: La clave fracasó en su intento de insertarse correctamente.");
			printf("Fallo al agregar a memtable.\n");
			return 2;
		}
		else
		{
			logInfo( "Lissandra: La clave fue insertada correctamente.");
			printf("Agregado correctamente.\n");
			return 0;
		}
	}
	tamanio_memtable = 0;
}

t_keysetter* selectKey(char* tabla, uint16_t receivedKey)
{
	int perteneceATabla(t_Memtablekeys* key)
	{
		char* testTable = string_new();
		testTable = malloc(strlen(tabla) + 1);
		strcpy(testTable, tabla);
		return 0 == strcmp(key->tabla, testTable);
	}

	int esDeTalKey(t_Memtablekeys* chequeada)
	{
		return chequeada->data->key == receivedKey;
	}

		if(existeTabla(tabla))
		{
			t_list* keysDeTablaPedida = list_create();
			t_list* keyEspecifica = list_create();
			t_Memtablekeys* auxMemtable;
			t_keysetter* keyTemps = selectKeyFS(tabla, receivedKey);
			t_keysetter* key;
			keysDeTablaPedida = list_filter(memtable, (void*)perteneceATabla);
			keyEspecifica = list_filter(keysDeTablaPedida, (void*)esDeTalKey);
			if(!list_is_empty(keysDeTablaPedida))
			{
				list_sort(keyEspecifica, (void*)chequearTimestamps);
				auxMemtable = list_get(keyEspecifica, 0);
				if(keyTemps != NULL)
				{
					if(chequearTimeKey(keyTemps, auxMemtable->data))
						key = keyTemps;
					else
						key = auxMemtable->data;
				}
				else
					key = auxMemtable->data;
			}
			else
			{
				if(keyTemps != NULL)
					key = keyTemps;
				else
					key = NULL;
			}

			list_destroy_and_destroy_elements(keysDeTablaPedida, &free);
			list_destroy(keyEspecifica);
			logInfo( "Lissandra: se ha obtenido la clave más actualizada en el proceso.");
			return key;
		}
		else
		{
			t_keysetter* key = NULL;
			return key;
		}

}

int llamadoACrearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	switch (crearTabla(nombre, consistencia, particiones, tiempoCompactacion))
	{
		case 0:
		{
			logInfo("Lissandra: se ha pedido al Compactador que inicie un hilo para gestionar la %s", nombre);
			gestionarTabla(nombre);
			return 0;
			break;
		}
		case 1:
		{
			return 1;
			break;
		}
		case 2:
		{
			return 2;
			break;
		}
		default:
			return 1;
		break;
	}
}

int llamarEliminarTabla(char* tablaPorEliminar)
{
	bool estaTabla(t_TablaEnEjecucion* tablaDeLista)
	{
		char* tablaAux = malloc(strlen(tablaPorEliminar) + 1);
		strcpy(tablaAux, tablaPorEliminar);
		int cantCarac = strlen(tablaDeLista->tabla);
		//char* tablaDeListaAux = string_new();
		char* tablaDeListaAux = malloc(cantCarac + 1);
		strcpy(tablaDeListaAux, tablaDeLista->tabla);
		bool result = (0 == strcmp(tablaDeListaAux, tablaAux));
		free(tablaDeListaAux);
		free(tablaAux);
		return result;
	}
	int result = dropTable(tablaPorEliminar);
	switch(result)
	{
	case 0:
	{
		list_remove_by_condition(tablasEnEjecucion, (void*) estaTabla);
		logInfo("Lissandra: Se ha removido a la %s de la lista de tablas en ejecución", tablaPorEliminar);
		break;
	}
	default:
		break;
	}
	return result;
}

int describirTablas(char* tablaSolicitada, bool solicitadoPorMemoria, char* buffer)
{
	char* tabla = malloc(strlen(tablaSolicitada) + 1);
	strcpy(tabla, tablaSolicitada);
	char* auxbuffer = string_new();
	if(0 == strcmp(tabla, ""))
	{
		logInfo( "Lissandra: Me llega un pedido de describir todas las tablas");
		int tablasExistentes = contarTablasExistentes();
		if(tablasExistentes == 0)
		{
			logError( "Lissandra: No existe ningún directorio en le FileSystem");
			printf("Error al acceder a todos los directorios");
			char* errormarker = "error";
			buffer = realloc(buffer, 6);
			memcpy(buffer, errormarker, strlen(errormarker));
			return 1;
		}
		else
		{
			if(!solicitadoPorMemoria)
			{
				mostrarTodosLosMetadatas(solicitadoPorMemoria, auxbuffer);
				return 0;
			}
			else
			{
				char* massiveBufferMetadatas = string_new();
				mostrarTodosLosMetadatas(solicitadoPorMemoria, massiveBufferMetadatas);
				int largoMassiveBuffer = strlen(massiveBufferMetadatas);
				buffer = realloc(buffer, largoMassiveBuffer + 1);
				strcpy(buffer, massiveBufferMetadatas);
				return 0;
			}
		}
	}
	else
	{
		int tamanio_buffer = 1;
		logInfo( "Lissandra: Me llega un pedido de describir la tabla %s", tabla);
		if(solicitadoPorMemoria)
		{
			tamanio_buffer = mostrarMetadataEspecificada(tabla, tamanio_buffer, solicitadoPorMemoria, auxbuffer);
			int sizebuffer = strlen(auxbuffer);
			buffer = realloc(buffer, sizebuffer + 1);
			strcpy(buffer, auxbuffer);
			return 0;
		}
		else
		{
			mostrarMetadataEspecificada(tabla, tamanio_buffer, solicitadoPorMemoria, auxbuffer);
			return 0;
		}
	}
	free(tabla);
}

void killProtocolLissandra()
{
	killthreads = true;
	list_destroy(hilos);
	list_destroy(memorias);
	list_destroy(compactadores);
	free(server_ip);
	logInfo("Lissandra: Todas las memorias han sido desalojadas.");
}

