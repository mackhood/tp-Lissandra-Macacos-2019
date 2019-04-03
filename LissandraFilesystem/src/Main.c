#include "Lissandra.h"

int main (void)
{
	char* lissandraFL_log_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFileSystem/LissandraFileSystem.log");
	loggerLFL = crearLogger(lissandraFL_log_ruta, "LissandraFileSystem");
	char* lissandraFL_config_ruta = strdup("/home/utnso/workspace/tp-2019-1c-Macacos/LissandraFileSystem/Lissandra.cfg");
	leerConfig(lissandraFL_config_ruta,loggerLFL);

	return EXIT_SUCCESS;
}
