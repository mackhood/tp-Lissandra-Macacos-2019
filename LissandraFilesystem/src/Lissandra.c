#include "Lissandra.h"

void setearValores(t_config * archivoConfig)
{
	retardo = config_get_int_value(archivoConfig, "RETARDO");
	tiempo_dump = config_get_int_value(archivoConfig, "TIEMPO_DUMP");
	puerto_de_escucha = config_get_int_value(archivoConfig, "PUERTO_ESCUCHA");
	punto_de_montaje = config_get_string_value(archivoConfig, "PUNTO_MONTAJE");
}
