#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread -lreadline -lm  -lcrypto"

echo "Compilando proyecto FileSystem";
gcc -I archivosBibliotecaCompartida *.c ${archivosBibliotecaCompartida} -o FileSystem.o ${referencias};

