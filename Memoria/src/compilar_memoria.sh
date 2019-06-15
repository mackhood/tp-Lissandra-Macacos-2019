#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread -lm -lreadline -lcrypto -lssl"

mkdir -p ../Debug

echo "Compilando proyecto Memoria";
gcc -I archivosBibliotecaCompartida *.c ${archivosBibliotecaCompartida} -o ../Debug/Memoria.o ${referencias};
echo "Success";