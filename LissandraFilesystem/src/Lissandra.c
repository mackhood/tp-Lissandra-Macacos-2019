#include "Lissandra.h"

void inicializar()
{
	hilos = list_create();
	memorias = list_create();
	compactadores = list_create();

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
		memcpy(&hora_actual, mensaje->payload+sizeof(uint16_t), sizeof(time_t));
		memcpy(&tamanio_value, mensaje->payload+sizeof(uint16_t)+sizeof(time_t), sizeof(int));

		char* value = malloc(tamanio_value+1);
		memcpy(value, mensaje->payload+sizeof(uint16_t)+sizeof(time_t)+sizeof(int), tamanio_value);
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

void escucharMemoria(int* socket_memoria){
	int socket = *socket_memoria;

	while(1){
		t_prot_mensaje* mensaje_memoria = prot_recibir_mensaje(socket);

		switch(mensaje_memoria->head){
		case SOLICITUD_TABLA:{
			time_t tiempo_pag = time(NULL);
			char* value = "value de prueba";
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




void insertKeysetter(char* tabla, uint16_t key, char* value, time_t timestamp)
{
/*	struct t_memtablekey* key = malloc(sizeof(t_memtablekey));
	key->keysetter = clave;
	strcpy(key->tabla, tabla);*/
}

t_keysetter selectKey(char* tabla, uint16_t receivedKey)
{
	t_keysetter key;
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


