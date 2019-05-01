#include "estructurasMemoria.h"

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
		uint16_t posible_key;
		memcpy(&posible_key, memoria_principal+ (tamanio_pag*pagina_a_buscar->offset) + sizeof(double), sizeof(uint16_t));

		return posible_key == key;
	}

	t_est_pag* est_pagina_buscada = (t_est_pag*)list_find(segmento_buscado->tabla_paginas.paginas, _seEncuentraKey);
	return est_pagina_buscada;
}


t_est_pag* buscarEinsertarEnMem(t_segmento* segmento, uint16_t key, time_t time_a_insertar, char* value_con_barraCero){
	int marco_disponible = buscarPaginaLibre();
	t_est_pag* nueva_est_pagina = malloc(sizeof(t_est_pag));

	nueva_est_pagina->offset = marco_disponible;
	nueva_est_pagina->flag = 0;

	//desplazo la memoria y copio en una posicion "libre" lo que tengo en time-key-value
	memcpy(memoria_principal+(tamanio_pag*marco_disponible), &time_a_insertar, sizeof(double));
	memcpy(memoria_principal+(tamanio_pag*marco_disponible)+sizeof(double), &key, sizeof(uint16_t));
	memcpy(memoria_principal+(tamanio_pag*marco_disponible)+sizeof(double)+sizeof(uint16_t), value_con_barraCero, tamanio_value);

	//marco frame como ocupado
	estados_memoria[marco_disponible] = OCUPADO;

	//agrego la estructura a la tabla de paginas del segmento en cuestion
	list_add(segmento->tabla_paginas.paginas, (t_est_pag*)nueva_est_pagina);

	return nueva_est_pagina; //el return lo hago nada mas para las pruebas
}

int buscarPaginaLibre(){

	for(int marco_disponible=0; marco_disponible<cant_paginas; marco_disponible++){
		if(estados_memoria[marco_disponible]==LIBRE)
			{
			return marco_disponible;
			}
		}
	int marco_disponible = aplicarLRU();
	return marco_disponible;
}


//LA VAMOS A USAR CUANDO USEMOS BUSCAR PAGINA LIBRE
int aplicarLRU(){
	return 0;
}



