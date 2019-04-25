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
		time_t hora_actual;
		int tamanio_value;

		memcpy(&key_recibida, mensaje->payload, sizeof(uint16_t));
		memcpy(&hora_actual, mensaje->payload + sizeof(uint16_t), sizeof(time_t));
		memcpy(&tamanio_value, mensaje->payload + sizeof(uint16_t) + sizeof(time_t), sizeof(int));

		char* value = malloc(tamanio_value+1);
		memcpy(value, mensaje->payload + sizeof(uint16_t) + sizeof(time_t) + sizeof(int), tamanio_value);
		value[tamanio_value] = '\0';

		printf("el CLIENTE es %s y nos manda de prueba la key %d y la hora %ld\n\n", value, key_recibida, hora_actual);

		prot_destruir_mensaje(mensaje);

		//log_info(logger, "[Conexiones]: Se conecto una Memoria");
		pthread_t RecibirMensajesEsi;
		/*Duplico la variable que tiene el valor del socket del cliente*/
		int* memoria = (int*) malloc (sizeof(int));
		*memoria = socket_memoria;
		pthread_create(&RecibirMensajesEsi,NULL, (void*)escucharMemoria, memoria);

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
				char* tabla;
				memcpy(&auxkey, mensaje_memoria->payload, sizeof(uint16_t));
				t_keysetter* helpinghand = selectKey(tabla, auxkey);
				time_t tiempo_pag = helpinghand->timestamp;
				char* value = helpinghand->clave;
				int tamanio_value = strlen(value);

				size_t tamanio_buffer = (sizeof(time_t)+tamanio_value+sizeof(int));
				void* buffer = malloc(tamanio_buffer);

				memcpy(buffer, &tiempo_pag, sizeof(time_t));
				memcpy(buffer+sizeof(time_t), &tamanio_value, sizeof(int));
				memcpy(buffer+sizeof(time_t)+sizeof(int), value, tamanio_value);

				prot_enviar_mensaje(socket, VALUE_SOLICITADO_OK, tamanio_buffer, buffer);
				break;
			}

		}
	}
}

void insertKeysetter(char* tablaRecibida, uint16_t keyRecibida, char* valueRecibido, time_t timestampRecibido)
{
	tamanio_memtable = memtable->elements_count;
	t_Memtablekeys* auxiliar = malloc(sizeof(t_Memtablekeys));
	t_keysetter* auxiliarprima = malloc(sizeof(t_keysetter));
	auxiliarprima->key = keyRecibida;
	auxiliarprima->clave = valueRecibido;
	auxiliarprima->timestamp = timestampRecibido;
	auxiliar->tabla = tablaRecibida;
	auxiliar->data = auxiliarprima;


	printf("%i, %s,", auxiliar->data->key, auxiliar->tabla);
	printf(" %s, %ld\n", auxiliar->data->clave, auxiliar->data->timestamp);

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
//	free(auxiliar);
	free(auxiliarprima);
}

t_keysetter* selectKey(char* tabla, uint16_t receivedKey)
{
		t_list* keysDeTablaPedida = list_create();
		t_Memtablekeys* auxA = malloc(sizeof(t_Memtablekeys));
		tablaAnalizada = malloc(strlen(tabla) + 1);
		strcpy(tablaAnalizada, tabla);
		keysDeTablaPedida = list_filter(memtable, (void*)perteneceATabla);
		list_sort(keysDeTablaPedida, (void*)chequearTimestamps);
		auxA = list_get(keysDeTablaPedida, 0);
		t_keysetter* key = malloc(sizeof(t_keysetter*));//comparadorDeKeys();
		key = auxA->data;
		free(auxA);
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
	return tablaAnalizada == key->tabla;
}

int chequearTimestamps(t_Memtablekeys* key1, t_Memtablekeys* key2)
{
	return !(key1->data->timestamp > key2->data->timestamp);
}

