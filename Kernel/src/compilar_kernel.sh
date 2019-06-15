#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread -lm -lreadline -lcrypto -lssl"

mkdir -p ../Debug

echo "Compilando proyecto Kernel";
gcc -I archivosBibliotecaCompartida *.c ${archivosBibliotecaCompartida} -o ../Debug/Kernel.o ${referencias};
echo "Success";