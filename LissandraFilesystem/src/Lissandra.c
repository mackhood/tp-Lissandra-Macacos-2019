#include "Lissandra.h"

void inicializar()
{
	hilos = list_create();
	memorias = list_create();
	compactadores = list_create();
	memtable = list_create();
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

		char* value = malloc(tamanio_value+1);
		memcpy(value, mensaje->payload + sizeof(uint16_t) + sizeof(double) + sizeof(int), tamanio_value);
		value[tamanio_value] = '\0';

		printf("el CLIENTE es %s y nos manda de prueba la key %d y la hora %lf\n\n", value, key_recibida, hora_actual);

		prot_destruir_mensaje(mensaje);
		log_info(loggerLFL, "[Lissandra]: Se conecto una Memoria");
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

	while(1)
	{
		t_prot_mensaje* mensaje_memoria = prot_recibir_mensaje(socket);

		switch(mensaje_memoria->head)
		{
			case SOLICITUD_TABLA:
			{
				uint16_t auxkey;
	//			char* tabla;
//				int tamanioNombre;
				memcpy(&auxkey, mensaje_memoria->payload, sizeof(uint16_t));
//				memcpy(&tamanioNombre, mensaje_memoria->payload + sizeof(uint16_t), sizeof(int));
//				tabla = malloc(tamanioNombre);
//				memcpy(tabla, mensaje_memoria->payload + sizeof(uint16_t) + sizeof(int), tamanioNombre);
//				t_keysetter* helpinghand = selectKey(tabla, auxkey);
//				double tiempo_pag = helpinghand->timestamp;
	//			char* value = helpinghand->clave;
		//		int tamanio_value = strlen(value);

				double tiempo_pag = getCurrentTime();
				char* value = "Ejemplo";
				int tamanio_value = strlen(value);

				size_t tamanio_buffer = (sizeof(double)+tamanio_value+sizeof(int));
				void* buffer = malloc(tamanio_buffer);

				memcpy(buffer, &tiempo_pag, sizeof(double));
				memcpy(buffer+sizeof(double), &tamanio_value, sizeof(int));
				memcpy(buffer+sizeof(double)+sizeof(int), value, tamanio_value);

				prot_enviar_mensaje(socket, VALUE_SOLICITADO_OK, tamanio_buffer, buffer);
				break;
			}

		}
	}
}

void insertKeysetter(char* tablaRecibida, uint16_t keyRecibida, char* valueRecibido, double timestampRecibido)
{
	tamanio_memtable = memtable->elements_count;
	t_Memtablekeys* auxiliar = malloc(sizeof(t_Memtablekeys) + 4);
	t_keysetter* auxiliarprima = malloc(sizeof(t_keysetter) + 3);
	auxiliarprima->key = keyRecibida;
	auxiliarprima->clave = valueRecibido;
	auxiliarprima->timestamp = timestampRecibido;
	auxiliar->tabla = tablaRecibida;
	auxiliar->data = auxiliarprima;


	printf("%i, %s,", auxiliar->data->key, auxiliar->tabla);
	printf(" %s, %lf\n", auxiliar->data->clave, auxiliar->data->timestamp);

	if(0 == existeTabla(tablaRecibida))
	{
		log_error(loggerLFL, "Lissandra: La tabla no existe, por lo que no puede insertarse una clave.");
		printf("Tabla no existente.\n");
	}
	else
	{
		log_info(loggerLFL, "Lissandra: Se procede a insertar la clave recibida en la Memtable.");
		list_add(memtable, auxiliar);
		if(tamanio_memtable == memtable->elements_count)
		{
			log_error(loggerLFL, "Lissandra: La clave fracasó en su intento de insertarse correctamente.");
			printf("Fallo al agregar a memtable.\n");
		}
		else
		{
			log_info(loggerLFL, "Lissandra: La clave fue insertada correctamente.");
			printf("Agregado correctamente.\n");
		}
	}
	tamanio_memtable = 0;
}

t_keysetter* selectKey(char* tabla, uint16_t receivedKey)
{


		t_list* keysDeTablaPedida = list_create();
		t_list* keyEspecifica = list_create();
		t_Memtablekeys* auxA = malloc(sizeof(t_Memtablekeys) + 4);
		tablaAnalizada = malloc(strlen(tabla) + 1);
		keyAnalizada = receivedKey;
		strcpy(tablaAnalizada, tabla);
		keysDeTablaPedida = list_filter(memtable, (void*)perteneceATabla);
		keyEspecifica = list_filter(keysDeTablaPedida, (void*)esDeTalKey);
		list_sort(keyEspecifica, (void*)chequearTimestamps);
		auxA = list_get(keyEspecifica, 0);

		//Acá hace falta implementar el compactador y las claves del FL, para eso, despues se llama a comparadorDeKeys();

		t_keysetter* key = malloc(sizeof(t_keysetter) + 3);
		key = auxA->data;
		list_destroy(keysDeTablaPedida);
		free(tablaAnalizada);
		log_info(loggerLFL, "Lissandra: se ha obtenido la clave más actualizada en el proceso.");
		return key;
}

int llamadoACrearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	switch (crearTabla(nombre, consistencia, particiones, tiempoCompactacion))
	{
		case 0:
		{
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
	return dropTable(tablaPorEliminar);
}

int perteneceATabla(t_Memtablekeys* key)
{
	char* testTable = string_new();
	testTable = malloc(strlen(tablaAnalizada) + 1);
	strcpy(testTable, tablaAnalizada);
	return 0 == strcmp(key->tabla, testTable);
}

int chequearTimestamps(t_Memtablekeys* key1, t_Memtablekeys* key2)
{
	return (key1->data->timestamp > key2->data->timestamp);
}

int esDeTalKey(t_Memtablekeys* chequeada)
{
	return chequeada->data->key == keyAnalizada;
}

int describirTablas(char* tablaSolicitada, bool solicitadoPorMemoria, void* buffer)
{
	char* tabla = string_new();
	tabla = malloc(strlen(tablaSolicitada));
	strcpy(tabla, tablaSolicitada);
	char* auxbuffer = string_new();
	if(strcmp(tabla, ""))
	{
		log_info(loggerLFL, "Lissandra: Me llega un pedido de describir todas las tablas");
		int tablasExistentes = contarTablasExistentes();
		if(tablasExistentes == 0)
		{
			log_error(loggerLFL, "Lissandra: No existe ningún directorio en le FileSystem");
			printf("Error al acceder a todos los directorios");
			char* errormarker = "error";
			memcpy(buffer, errormarker, strlen(errormarker));
			return 1;
		}
		else
		{
			if(solicitadoPorMemoria)
			{
				mostrarTodosLosMetadatas(solicitadoPorMemoria, auxbuffer);
				return 0;
			}
			else
			{
				char* massiveBufferMetadatas = string_new();
				massiveBufferMetadatas = malloc((sizeof(int) * 2 + 6) * tablasExistentes);
				mostrarTodosLosMetadatas(solicitadoPorMemoria, massiveBufferMetadatas);
				return 0;
			}
		}
	}
	else
	{
		log_info(loggerLFL, "Lissandra: Me llega un pedido de describir la tabla %s", tabla);
		if(solicitadoPorMemoria)
		{
			mostrarMetadataEspecificada(tabla, solicitadoPorMemoria, auxbuffer);
			return 0;
		}
		else
		{
			mostrarMetadataEspecificada(tabla, solicitadoPorMemoria, auxbuffer);
			return 0;
		}
	}
	free(tabla);
}

