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
