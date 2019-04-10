#!/bin/bash
archivosBibliotecaCompartida=$(cd ../../SharedLibrary; ls $PWD/*.c)
referencias="-lcommons -lpthread"

echo "Compilando proyecto Kernel";
gcc *.c ${archivosBibliotecaCompartida} -o Kernel.o ${referencias};
salida=$($?);
if {salida != 0}; then 
echo "Fallo la compilacion"
else
echo "Compilo piola"
fi

