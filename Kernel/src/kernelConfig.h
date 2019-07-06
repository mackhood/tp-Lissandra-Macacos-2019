/*
 * kernelConfig.h
 *
 *  Created on: 2 abr. 2019
 *      Author: utnso
 */

#ifndef KERNELCONFIG_H_
#define KERNELCONFIG_H_

#include <commons/collections/list.h>
#include <commons/collections/queue.h>
#include <commons/config.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>
#include "../../SharedLibrary/conexiones.h"
#include "../../SharedLibrary/loggers.h"

pthread_mutex_t mutexIdGDT;
pthread_mutex_t mutexIdMemoria;
pthread_mutex_t sc;
pthread_mutex_t ec;
pthread_mutex_t shc;
pthread_mutex_t memoriasCriterio;
pthread_mutex_t memoriasSinCriterio;
pthread_mutex_t memoriasCola;
pthread_mutex_t  memoriaConfig;

typedef struct {


	t_list* tablas;

}metadata;


typedef struct {
	char* nombre;
	char* criterio;
}__attribute__((packed)) t_tabla;




bool destProtocol;



typedef struct {

double Read_Latency; // 30s: El tiempo promedio que tarda un SELECT en ejecutarse en los últimos 30 segundos.
double Write_Latency; // 30s: El tiempo promedio que tarda un INSERT en ejecutarse en los últimos 30 segundos.
int Reads ;// 30s: Cantidad de SELECT ejecutados en los últimos 30 segundos.
int Writes; // 30s: Cantidad de INSERT ejecutados en los últimos 30 segundos.
//int Memory_Load [];  //(por cada memoria):  Cantidad de INSERT / SELECT que se ejecutaron en esa memoria respecto de las operaciones totales.
//int  Memory_Load_Select [];
//int Memory_Load_Insert [];


}estadisticas;

estadisticas * t_estadisticas;


typedef struct  {

//int conexion;
int numeroMemoria;
int estaEjecutando;
int puerto;
char * ip;
estadisticas * estadisticasMemoriaSC;
estadisticas * estadisticasMemoriaSHC;
estadisticas * estadisticasMemoriaEC;
int insertsTotales;
int selectTotales;


}memoria;



memoria* configuracion;


typedef struct {


	memoria* strongConsistency;
	t_list* StrongHash;
	t_queue* eventualConsistency;


}criterios;


typedef struct{
	int	puerto_memoria;
	int	quantum;
	int multiprocesamiento;
	char* ip_memoria;
	int metadata_refresh;
	int sleep_ejecucion;


}kernel_config;

typedef struct {

	kernel_config* config;
	int cantMemoriasPool;
	int primerConexion; //falg utilizado para conectarse por primera vez
	t_list* memoriasSinCriterio;
	t_list* memoriasConCriterio;
	t_list* memorias;
	t_queue * memoriasCola;
}t_kernel;

criterios* t_Criterios;

metadata* tMetadata;

time_t randomNumbertime;
t_kernel* tKernel;


typedef struct {

	int enteros [5];
	char *arreglo[10];
}params;



typedef struct{

	int idGDT;	 //Id dtb
	int flag; //flag de inicializacion
	char* path; // ruta del escriptorio
	int quantum; //dato el ultimo algoritmo del hito presenciar, el quantum puede cambiar por DTB
	int pc;
	int posMemoria;

	int total_sentencias;
	char* sentencias;

	bool se_ejecuto;
	double tiempo_repuesta;
	time_t horacreacion;  //se usa para calcular el tiempo de repuesta
	t_header instruccion_actual;
	params * parametros;
	t_header operacionActual;
	char* tablaSentencias [100];
	int sentenciaActual;
}DTB_KERNEL;



typedef struct {
	int idGDT;
	t_list* recursos;
}GDT;



bool  estaEnMetadata(char *);


GDT* buscar_gdt_by_id(int pid);
void agregarGDT(GDT* gdt);
GDT* crearGdt(int idGdt);
void destroyGdt(GDT* gdt);
void removeGDT(GDT* gdt);

typedef struct {
	int valor;
	char* recurso;
	t_queue* en_espera;
}t_recurso;



typedef struct {
	char* path;
	int posMemoria;
}t_archivoAbierto;

t_dictionary* tabla_recursos;
t_list* tablaGDT;
pthread_mutex_t mutextListGDT;
pthread_mutex_t mutextListRecurso;




t_recurso* extraer_recurso(char* recurso);

#endif /* KERNELCONFIG_H_ */
