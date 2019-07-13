#include "Notify.h"

void notify()
{
	int length;
	int i = 0;
	char buffer[BUF_LEN];

	int fileToWatch = inotify_init();
	if( fileToWatch < 0)
	{
		logError("[NOTIFY/MEMORIA]: Error al iniciar el notifier.");
		return;
	}

	int watchDescriptor = inotify_add_watch(fileToWatch, PATH_CONFIG, IN_CREATE | IN_MODIFY | IN_DELETE | IN_DELETE_SELF);
	length = read(fileToWatch, buffer, BUF_LEN);

	if(length < 0)
	{
		logError("[NOTIFY/MEMORIA]: Error al tratar de acceder al archivo a vigilar.");
		return;
	}

	while(i < length)
	{
		struct inotify_event* event = (struct inotify_event*) &buffer[i];
		if(event->mask & IN_MODIFY)
		{
			sleep(2);
			logInfo("[NOTIFY/MEMORIA]: se ha modificado el archivo de configuración, actualizando valores.");
			t_config* ConfigMain = config_create(PATH_CONFIG);
			info_memoria.retardo_fs = config_get_int_value(ConfigMain, "RETARDO_FS");
			info_memoria.retardo_mp = config_get_int_value(ConfigMain, "RETARDO_MEM");
			info_memoria.tiempo_goss = config_get_int_value(ConfigMain, "RETARDO_GOSSIPING");
			info_memoria.tiempo_jour = config_get_int_value(ConfigMain, "RETARDO_JOURNAL");

			config_destroy(ConfigMain);
			logInfo("[NOTIFY/MEMORIA]: valores actualizados.");
			puts("Al detectarse un cambio en el archivo de configuración, se actualizaron los valores de Memoria.");

		}
		else if(event->mask & IN_DELETE_SELF)
		{
			logInfo("[NOTIFY/MEMORIA]: Se ha detectado que el archivo de config ha sido eliminado. Terminando sistema.");
			puts("Memoria está siendo desconectado ya que el archivo de configuración fue destruido.");
			break;
		}
		else if(event->mask & IN_CREATE)
		{
			logInfo("[NOTIFY/MEMORIA]: Se ha detectado el archivo de configuración");
		}
		length = read(fileToWatch, buffer, BUF_LEN);
	}
	(void) inotify_rm_watch(fileToWatch, watchDescriptor);
	(void) close(fileToWatch);
}

