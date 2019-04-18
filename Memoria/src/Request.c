#include "Request.h"


void selectt (char** args) {
	char* nombre_tabla = string_duplicate(args[1]);
	uint16_t key = atoi(args[2]);

	if(/*seEncuentranEnMemoria(nombre_tabla, key)*/1){
		//Lo busco en el segmento

	}
	else{
		//se lo pido al fs
	}
}

void insert (char** args) {





}
void create (char** args) {





}
void describe (char** args) {





}
void drop (char** args) {





}
void journal (char** args) {





}
void add (char** args) {





}
void run (char** args) {





}
void readLatency (char** args) {





}
void writeLatency (char** args) {





}
void reads (char** args) {





}
void writes (char** args) {





}
void memoryLoad (char** args) {





}
