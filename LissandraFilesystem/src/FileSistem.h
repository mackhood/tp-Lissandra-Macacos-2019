#ifndef FILESISTEM_H_
#define FILESISTEM_H_

#include "LFLExternals.h"


/* VARIABLES GLOBALES */

int tamanio_bloques;
int creatingFL;
int tableWatchDescriptor;
int tableDirectoryToWatch;
int blockDirectoryToWatch;
int blockWatchDescriptor;

/* FUNCIONES */

void mainFileSistem();
void setearValoresFileSistem(t_config * archivoConfig);
void testerFileSystem();
void levantarBitmap(char* direccion);
int crearTabla(char* nombre, char* consistencia, int particiones, int tiempoCompactacion);
int crearMetadata(char* direccionFinal, char* consistencia, int particiones, int tiempoCompactacion);
int crearParticiones(char* direccionFinal, int particiones);
int dropTable(char* tablaPorEliminar);
int limpiadorDeArchivos(char* direccion, char* tabla);
int existeTabla(char* tabla);
char* mostrarMetadataEspecificada(char* tabla, bool solicitadoPorMemoria);
t_list* mostrarTodosLosMetadatas(bool solicitadoPorMemoria, char* buffer);
int contarTablasExistentes();
t_keysetter* selectKeyFS(char* tabla, uint16_t keyRecibida);
char* escribirBloquesDeFs(char* todasLasClavesAImpactar, int tamanioUsado, char* tabla);
char* obtenerBloqueLibre();
void escribirBloque(int* usedBlocks, int* seizedSize, int usedSize, char* block, char* clavesAImpactar);
void limpiadorDeBloques(char* direccion);
void limpiarBloque(char* direccionPart);
char* leerBloque(char* bloque);
char** obtenerBloques(char* direccion);
void initTablesNotifier();
void initBlocksNotifier();
void fileSystemNotifier();
void tablesNotifier();
void killProtocolFileSystem();


#endif /* FILESISTEM_H_ */
