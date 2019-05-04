/*
 * auxiliaryFunctions.c
 *
 *  Created on: 26 abr. 2019
 *      Author: utnso
 */

#include "auxiliaryFunctions.h"

double getCurrentTime()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	unsigned long long result = (((unsigned long long)tv.tv_sec) * 1000 + ((unsigned long long)tv.tv_usec) / 1000);
	double a = result;
	return a;
}


