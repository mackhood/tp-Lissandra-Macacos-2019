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

void liberadorDeArrays(char** array)
{
	int count = 0;
	while(array[count] != NULL)
	{
		free(array[count]);
		count++;
	}
	free(array);
}

int itsANumber(char* check)
{
	int recount = 0;
	while(check[recount] != NULL)
	{
		if(!isdigit(check[recount]))
		{
			return 0;
		}
		recount++;
	}
	return 1;
}

int doesNotContainNumber(char* check)
{
	int recount = strlen(check);
	int i = 0;
	for(i = 0; i < recount; i++)
	{
		if(isdigit(check[recount]))
		{
			return 0;
		}
	}
	return 1;
}

int criterioInvalido(char* check)
{
	if(!strcmp(check, "SC") || !strcmp(check, "EC") || !strcmp(check, "SHC"))
		return 0;
	else
		return 1;
}

