#ifndef AUXILIARYFUNCTIONS_H_
#define AUXILIARYFUNCTIONS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

double getCurrentTime() ;
void liberadorDeArrays(char** array);
int itsANumber(char* check);
int doesNotContainNumber(char* check);
int criterioInvalido(char* check);

#endif /* AUXILIARYFUNCTIONS_H_ */
