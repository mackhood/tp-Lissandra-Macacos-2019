/*
 * configs.h
 *
 *  Created on: 1 abr. 2019
 *      Author: utnso
 */

#ifndef CONFIGS_H_
#define CONFIGS_H_


#include<commons/log.h>
#include<commons/config.h>


void leerConfig(char * configPath,t_log* logger);
void setearValores(t_config * archivoConfig); // Redefinir en cada proyecto
void leerArchivoDeConfiguracion(char * configPath,t_log* logger);




#endif /* CONFIGS_H_ */
