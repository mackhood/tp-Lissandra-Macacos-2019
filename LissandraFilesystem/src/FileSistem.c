#include "FileSistem.h"

void mainFileSistem()
{

}

void setearValoresFileSistem(t_config * archivoConfig)
{
	punto_montaje = config_get_string_value(archivoConfig, "PUNTO_MONTAJE");
}

void crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion)
{
	char* direccion = strcat( punto_montaje , nombre);
	mkdir(direccion, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
}
