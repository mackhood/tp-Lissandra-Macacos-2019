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
	DIR* newdir;
	char buff[128];
	memset(buff,0,sizeof(buff));
	strcpy(buff, punto_montaje);

	if(NULL == (newdir = opendir(punto_montaje)))// reviso si el punot de montaje es accesible
	{
		log_info(loggerLFL,"FileSistem: El directorio que usted desea crear no es accesible");
		exit(1);
	}
	else
	{
		if(buff[strlen(buff-1)]=='/') //reviso si me pusieron una barra al final de la dirección
		{
			char* nombreconbarra = strcat(nombre,"/");
			strncpy(buff + strlen(buff),nombreconbarra,7);
		}
		else
		{
			char* nombreconbarra = strcat(nombre,"/");
			char* nombrecondosbarras = strcat("/",nombreconbarra);
			strncpy(buff + strlen(buff),nombrecondosbarras,8);
		}
		log_info(loggerLFL, "FileSistem: Se procede a la creacion del directorio");
		mkdir(buff, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); //creo el directorio de la tabla con sus respectivos permisos
		closedir(newdir);
	}

}
