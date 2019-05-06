/*
 * LFLExternals.c
 *
 *  Created on: 6 may. 2019
 *      Author: utnso
 */

#include "LFLExternals.h"

int chequearTimestamps(t_Memtablekeys* key1, t_Memtablekeys* key2)
{
	return (key1->data->timestamp > key2->data->timestamp);
}

