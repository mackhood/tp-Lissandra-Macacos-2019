/*
 * loggers.h
 *
 *  Created on: 1 abr. 2019
 *      Author: utnso
 */

#ifndef LOGGERS_H_
#define LOGGERS_H_

#include<commons/log.h>



#include <commons/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <commons/string.h>
#include <stdbool.h>

//TIPOS DE NIVEL PARA LOGGEAR

#define LOG_FILE "LOG_FILE"
#define LOG_LEVEL "LOG_LEVEL"
#define LOG_PROGRAM_NAME "LOG_PROGRAM_NAME"
#define LOG_PRINT_CONSOLE "LOG_PRINT_CONSOLE"

t_log* crearLogger(char * dir_log, char * logMemoNombreArch);

void logDestroy();
void initMutexlog(char* archivo,char* nombrePrograma,bool activarConsola,t_log_level level);
void logError(const char* string, ...);
void logWarning(const char* string, ...);
void logTrace(const char* string, ...);
void logDebug(const char* string, ...);
void logInfo(const char* string, ...);


t_log* iniciar_logger(char* );





#endif /* LOGGERS_H_ */
