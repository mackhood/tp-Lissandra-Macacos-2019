#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread"

echo "Compilando proyecto Memoria";
gcc *.c ${archivosBibliotecaCompartida} -o Memoria.o ${referencias};

