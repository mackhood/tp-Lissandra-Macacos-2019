#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread -lm -lreadline -lcrypto -lssl"

echo "Compilando proyecto Kernel";
gcc *.c ${archivosBibliotecaCompartida} -o Kernel.o ${referencias};


