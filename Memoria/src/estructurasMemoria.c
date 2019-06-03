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


t_segmento* buscarEinsertarEnMem(t_segmento* segmento, uint16_t key, double time_a_insertar, char* value_con_barraCero){
	char* nombre_tabla = strdup(segmento->nombre_tabla);

	int marco_disponible = buscarPaginaLibre();

	//PREGUNTAR
	if(se_hizo_journal){
		segmento = malloc(sizeof(t_segmento));
		segmento->nombre_tabla = nombre_tabla;
		segmento->tabla_paginas.paginas = list_create();
		//list_add(lista_segmentos, segmento);

		//se_hizo_journal = false;
	}

	t_est_pag* nueva_est_pagina = malloc(sizeof(t_est_pag));

	nueva_est_pagina->offset = marco_disponible;
	nueva_est_pagina->flag = 0;

	//desplazo la memoria y copio en una posicion "libre" lo que tengo en time-key-value
	memcpy(memoria_principal+(tamanio_pag*marco_disponible), &time_a_insertar, sizeof(double));
	memcpy(memoria_principal+(tamanio_pag*marco_disponible)+sizeof(double), &key, sizeof(uint16_t));
	//el tamanio_value es el maximo tamanio del value el cual nos lo va a enviar el fs desde un principio
	memcpy(memoria_principal+(tamanio_pag*marco_disponible)+sizeof(double)+sizeof(uint16_t), value_con_barraCero, tamanio_value);

	//marco frame como ocupado
	estados_memoria[marco_disponible] = OCUPADO;

	//agrego la estructura a la tabla de paginas del segmento en cuestion
	list_add(segmento->tabla_paginas.paginas, (t_est_pag*)nueva_est_pagina);

	return segmento;
}

int buscarPaginaLibre(){

	for(int marco_disponible=0; marco_disponible<cant_paginas; marco_disponible++){
		if(estados_memoria[marco_disponible]==LIBRE)
			{
			printf("el marco a asignar es %d\n", marco_disponible);
			return marco_disponible;
			}
	}
	//sali del for por ende estan todos los marcos ocupados
	printf("aplico LRU debido a que estan todos los marcos ocupados\n");
	int marco_disponible = aplicarLRU();
	return marco_disponible;
}


//LA VAMOS A USAR CUANDO USEMOS BUSCAR PAGINA LIBRE
int aplicarLRU(){
	//dentro va a estar el journaling

	//declaro el time, la pag, el segmento y la pos de la pag en tp del seg debido a que los asignare a futuro para la operatoria
	//t_est_pag* pag_mas_vieja;
	double time_mas_viejo = getCurrentTime();
	t_segmento* seg_con_pag_mas_vieja;
	int pos_pag_en_tp_seg;
	bool hay_pag_a_liberar = false;

	int cant_segmentos = list_size(lista_segmentos);

	//No pregunto si hay segmentos debido a que si hago el LRU es porque CLARAMENTE hay segmentos, lo mismo con las paginas
	for(int i = 0; i< cant_segmentos; i++){
		t_segmento* segmento_a_evaluar = list_get(lista_segmentos, i);
		int cant_paginas_segmento = list_size(segmento_a_evaluar->tabla_paginas.paginas);

		for(int j = 0; j< cant_paginas_segmento; j++){
			t_est_pag* pagina_a_evaluar = list_get(segmento_a_evaluar->tabla_paginas.paginas, j);
			double time_pag_a_evaluar;
			memcpy(&time_pag_a_evaluar, memoria_principal+tamanio_pag*(pagina_a_evaluar->offset), sizeof(double));

			if( (pagina_a_evaluar->flag == 0) && (time_pag_a_evaluar < time_mas_viejo) ){
				time_mas_viejo = time_pag_a_evaluar;
				//pag_mas_vieja = pagina_a_evaluar;
				seg_con_pag_mas_vieja = segmento_a_evaluar;
				pos_pag_en_tp_seg = j;
				hay_pag_a_liberar = true;
			}
		}
	}

	//si salgo del for y no hay pagina a liberar significa que todas las paginas de todos los segmentos se encuentran modificados (FULL) o que algo raro pasó
	if(!hay_pag_a_liberar){
		printf("Aplico Journal debido a que mi memoria se encuentra FULL\n");
		journal();
		se_hizo_journal = true;

		return 0;
	}
	else{
		//significa que encontre la pag mas vieja y tiene flag == 0
		//devuelvo la misma pagina que pag_mas_vieja

		t_est_pag* pagina_a_remover = (t_est_pag*)list_remove(seg_con_pag_mas_vieja->tabla_paginas.paginas, pos_pag_en_tp_seg);
		estados_memoria[pagina_a_remover->offset] = LIBRE;
		int marco_libre = pagina_a_remover->offset;
		printf("Encontre la pag mas vieja con flag en 0 y nos habilitara el marco %d\n", marco_libre);
		free(pagina_a_remover);

		return marco_libre;
	}
}

void eliminar_segmentos(){

	int cant_segmentos = list_size(lista_segmentos);

	for(int i = 0; i<cant_segmentos; i++){

		t_segmento* segmento = list_get(lista_segmentos, i);

		//creo que usando free libero todas las posiciones
		t_list* paginas_segmento = segmento->tabla_paginas.paginas;
		list_destroy_and_destroy_elements(paginas_segmento, &free);
		//list_destroy(paginas_segmento); //--> asi funcionaba
	}

	list_clean_and_destroy_elements(lista_segmentos, &free);
	//list_clean(lista_segmentos); //asi funcionaba pero tengo que liquidar lo de adentro tambien
}

void liberar_marcos(){

	for(int marco_ocupado=0; marco_ocupado<cant_paginas; marco_ocupado++){
			if(estados_memoria[marco_ocupado]==OCUPADO)
				{
					estados_memoria[marco_ocupado] = LIBRE;
				}
			}
}

int buscarPosSeg(char* nombre_segmento){
	int cant_segmentos = list_size(lista_segmentos);
	int posicion;

	for(int i=0; i<cant_segmentos; i++){
		t_segmento* segmento_a_chequear = list_get(lista_segmentos, i);
		if(strcmp(nombre_segmento, segmento_a_chequear->nombre_tabla)==0){
			posicion = i;
			break;
		}
	}
	return posicion;
}


/*LRU
 *
 *  time mas viejo = tiempo actual
 *  for(cant segmentos){
 *  	obtengo segmento
 * 	 	for(cantidad paginas de segmento obtenido){
 *	 		if(pagina.flag == 0 && pagina.time < time_mas_viejo){
 *				time_mas_viejo = pagina.time
 *				t_est_pagina* pagina_mas_vieja = list_get(segmento->paginas, contador);
 *			}
 *		}
 *	}
 *
 *	si salgo del for y mi time sigue en 0 significa que todas las paginas de todos los segmentos se encuentran modificados o que algo raro pasó
 *	if(time_mas_viejo == 0){
 *		Aplicar JOURNALING --> devuelve 0 luego de realizar el journaling?
 *	}
 *	else{
 *		//quito la pagina de memoria principal
 *
 *		//devuelvo el marco de la pagina
 *		return pagina_mas_vieja.marco
 *	}
 *
 */

