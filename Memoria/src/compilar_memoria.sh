#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread -lm -lreadline -lcrypto -lssl"

echo "Compilando proyecto Memoria";
gcc *.c ${archivosBibliotecaCompartida} -o Memoria.o ${referencias};

