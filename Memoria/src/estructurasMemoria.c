
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


t_est_pag* buscarEinsertarEnMem(t_segmento* segmento, uint16_t key, time_t time_a_insertar, int tamanio_value, char* value){
	t_pagina* pagina_libre = buscarPaginaLibre();
	t_est_pag* nueva_est_pagina = malloc(sizeof(t_est_pag));

	nueva_est_pagina->pagina = pagina_libre;
	nueva_est_pagina->pagina->key = key;
	nueva_est_pagina->pagina->timestamp = time_a_insertar;
	memcpy(nueva_est_pagina->pagina->value, value, tamanio_value);
	nueva_est_pagina->pagina->value[tamanio_value+1] = '\0';
	list_add(segmento->tabla_paginas.paginas, (t_est_pag*)nueva_est_pagina);

	return nueva_est_pagina; //el return lo hago nada mas para las pruebas
}

t_pagina* buscarPaginaLibre(){
	int i=0;
	while(estados[i]==OCUPADO && i<cant_paginas){
		i++;
	}

	if(estados[i]==LIBRE){
		return &(memoria_principal[i]);
	}
	else{
		int posicion_libre = aplicarLRU();
		return &(memoria_principal[posicion_libre]);
	}
}

//LA VAMOS A USAR CUANDO USEMOS BUSCAR PAGINA LIBRE
int aplicarLRU(){
	return 0;
}


