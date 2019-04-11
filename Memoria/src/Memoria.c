#include "Memoria.h"

int main() {

	levantar_config();

	//NO BORRAR DEBIDO A QUE LO USAMOS EN BREVE
	/*socket_kernel = levantar_servidor(info_memoria.puerto);
	t_prot_mensaje* handshake_kernel = prot_recibir_mensaje(socket_kernel);
	printf("el mensaje es %s\n", (char*)handshake_kernel->payload);


	socket_fs = conectar_a_servidor(info_memoria.ip_fs, info_memoria.puerto_fs, "Memoria");*/

	while(1);
	return EXIT_SUCCESS;
}
