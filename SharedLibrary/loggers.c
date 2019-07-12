/*
 * loggers.c
 *
 *  Created on: 1 abr. 2019
 *      Author: utnso
 */


#include "loggers.h"

t_log* iniciar_logger(char* rutaArchivo) {
	return log_create(rutaArchivo, "Logger", 1, LOG_LEVEL_INFO);
}



pthread_mutex_t mutexLog;
t_log* logger;

t_log* crearLogger(char * dir_log, char * logMemoNombreArch) {

	t_log * logger = log_create(dir_log, logMemoNombreArch, false, LOG_LEVEL_INFO);
	return logger;
}

void initMutexlog(char* archivo,char* nombrePrograma,bool activarConsola,t_log_level level){
	pthread_mutex_init(&mutexLog,NULL);
	logger = log_create(archivo,nombrePrograma,activarConsola,level);

}
void logDestroy(){
	log_destroy(logger);
	pthread_mutex_destroy(&mutexLog);
}

void logInfo(const char* mensaje, ...){
	pthread_mutex_lock(&mutexLog);
	va_list arg;
	va_start(arg,mensaje);

	char* info = string_from_vformat(mensaje,arg);
	log_info(logger, info);
	va_end(arg);
	pthread_mutex_unlock(&mutexLog);
	free(info);
}

void logDebug(const char* mensaje, ...){
	pthread_mutex_lock(&mutexLog);
	va_list arg;
	va_start(arg,mensaje);

	char* mensajeLog = string_from_vformat(mensaje,arg);
	log_debug(logger,mensajeLog);
	free(mensajeLog);
	va_end(arg);
	pthread_mutex_unlock(&mutexLog);
}

void logTrace(const char* mensaje, ...){
	pthread_mutex_lock(&mutexLog);
	va_list arg;
	va_start(arg,mensaje);

	char* mensajeLog = string_from_vformat(mensaje,arg);
	log_trace(logger,mensajeLog);
	free(mensajeLog);
	va_end(arg);
	pthread_mutex_unlock(&mutexLog);
}

void logWarning(const char* mensaje, ...){
	pthread_mutex_lock(&mutexLog);
	va_list arg;
	va_start(arg,mensaje);

	char* mensajeLog = string_from_vformat(mensaje,arg);
	log_warning(logger,mensajeLog);
	free(mensajeLog);
	va_end(arg);
	pthread_mutex_unlock(&mutexLog);
}

void logError(const char* mensaje, ...){
	pthread_mutex_lock(&mutexLog);
	va_list arg;
	va_start(arg,mensaje);

	char* mensajeLog = string_from_vformat(mensaje,arg);
	log_error(logger,mensajeLog);
	free(mensajeLog);
	va_end(arg);
	pthread_mutex_unlock(&mutexLog);
}
