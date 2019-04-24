#include "estructurasMemoria.h"

//ya no uso mas esta funcion
t_pagina* estaTablaYkeyEnMemoria(char* tabla_a_buscar, uint16_t key){

	t_segmento* segmento_buscado = buscarSegmento(tabla_a_buscar);

	if(segmento_buscado /*!= NULL*/){
		t_est_pag* est_pagina_buscada = buscarEstPagBuscada(key, segmento_buscado);
		t_pagina* pagina_buscada = est_pagina_buscada->pagina;
		if(pagina_buscada /*!= NULL*/){
			return pagina_buscada;
		}
	}
	return NULL;
}

t_segmento* buscarSegmento(char* tabla_a_buscar){
	//declaro una funcion dentro de estaEnMemoria para poder buscar primero el segmento en la tabla de segmentos
	bool _seEncuentraNombre(void* segmento){
		t_segmento* segmento_a_buscar = (t_segmento*)segmento;
		return strcmp(segmento_a_buscar->nombre_tabla, tabla_a_buscar) == 0;
	}

	t_segmento* segmento_buscado = (t_segmento*)list_find(lista_segmentos, _seEncuentraNombre);
	return segmento_buscado;
}

t_est_pag* buscarEstPagBuscada(uint16_t key, t_segmento* segmento_buscado){
	//declaro una funcion dentro de estaEnMemoria para poder buscar la pagina en la tabla de paginas del segmento encontrado
	bool _seEncuentraKey(void* est_pagina){
		t_est_pag* pagina_a_buscar = (t_est_pag*)est_pagina;
		return pagina_a_buscar->pagina->key == key;
	}

	t_est_pag* est_pagina_buscada = (t_est_pag*)list_find(segmento_buscado->tabla_paginas.paginas, _seEncuentraKey);
	return est_pagina_buscada;
}

t_est_pag* crearPagina(time_t tiempo_de_pag, uint16_t key, int tamanio_value, char* value){
	t_est_pag* est_pag_nuevo = malloc(sizeof(t_est_pag));
	est_pag_nuevo->flag = 0;
	est_pag_nuevo->pagina = malloc(sizeof(t_pagina));
	est_pag_nuevo->pagina->key = key;
	est_pag_nuevo->pagina->timestamp = tiempo_de_pag;
	est_pag_nuevo->pagina->value = malloc(tamanio_value+1); //+1 por el barra 0
	memcpy(est_pag_nuevo->pagina->value, value, tamanio_value);
	est_pag_nuevo->pagina->value[tamanio_value] = '\0';

	return est_pag_nuevo;
}

void chequearLugaresEinsertar(t_segmento* segmento, t_est_pag* est_pag_a_ins){

	if(cant_lugares>0){
		list_add(segmento->tabla_paginas.paginas, (t_est_pag*)est_pag_a_ins);
		cant_lugares--;
	}
	else{
		aplicarLRU();
		list_add(segmento->tabla_paginas.paginas, (t_est_pag*)est_pag_a_ins);
	}
}

void aplicarLRU(){
	return;
}
