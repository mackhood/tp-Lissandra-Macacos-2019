#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread -lreadline -lm  -lcrypto"

mkdir -p ../Debug

echo "Compilando proyecto LissandraFileSystem";
gcc -I archivosBibliotecaCompartida *.c ${archivosBibliotecaCompartida} -o ../Debug/LissandraFileSystem.o ${referencias};
echo "Success";
