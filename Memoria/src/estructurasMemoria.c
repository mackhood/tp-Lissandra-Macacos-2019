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

	logInfo("[estructurasMemoria]: Comenzara la busqueda de un frame libre");
	int marco_disponible = buscarPaginaLibre();

	/*	Siempre marcamos el se_inserta_segmento en true, si se realiza journal este quedara en false hasta que haya una nueva peticion
	*	de buscarEinsertar. Cabe destacar que se utiliza por si queremos agregar un segmento nuevo en memoria y el mismo tira journal,
	*	en este caso lo agrega dentro del condicional y en la request (es complejo y tiene un problema MINIMO de memory leak)
	*/

	se_inserta_segmento = true;
	if(se_hizo_journal){
		segmento = malloc(sizeof(t_segmento));
		segmento->nombre_tabla = nombre_tabla;
		segmento->tabla_paginas.paginas = list_create();

		list_add(lista_segmentos, segmento);

		se_inserta_segmento = false;
		se_hizo_journal = false;
	}

	logInfo("[estructurasMemoria]: Creo nueva pagina con flag en 0 y le asigno el marco disponible");

	t_est_pag* nueva_est_pagina = malloc(sizeof(t_est_pag));

	nueva_est_pagina->offset = marco_disponible;
	nueva_est_pagina->flag = 0;

	logInfo("[estructurasMemoria]: copio en el marco %d los datos de la pagina en cuestion", marco_disponible);
	//desplazo la memoria y copio en una posicion "libre" lo que tengo en time-key-value
	memcpy(memoria_principal+(tamanio_pag*marco_disponible), &time_a_insertar, sizeof(double));
	memcpy(memoria_principal+(tamanio_pag*marco_disponible)+sizeof(double), &key, sizeof(uint16_t));
	//el tamanio_value es el maximo tamanio del value el cual nos lo va a enviar el fs desde un principio
	memcpy(memoria_principal+(tamanio_pag*marco_disponible)+sizeof(double)+sizeof(uint16_t), value_con_barraCero, tamanio_value);

	logInfo("[estructurasMemoria]: Marco el frame como ocupado");
	//marco frame como ocupado
	estados_memoria[marco_disponible] = OCUPADO;

	logInfo("[estructurasMemoria]: agrego la pagina a la tabla de paginas del segmento");
	//agrego la estructura a la tabla de paginas del segmento en cuestion
	list_add(segmento->tabla_paginas.paginas, (t_est_pag*)nueva_est_pagina);

	return segmento;
}

int buscarPaginaLibre(){

	for(int marco_disponible=0; marco_disponible<cant_paginas; marco_disponible++){
		if(estados_memoria[marco_disponible]==LIBRE)
			{
			logInfo("[estructurasMemoria]: el marco a asignar es %d", marco_disponible);
			return marco_disponible;
			}
	}
	//sali del for por ende estan todos los marcos ocupados
	logInfo("[estructurasMemoria]: Todos los marcos se encuentran ocupados por lo que se realizará el LRU");
	int marco_disponible = aplicarLRU();
	return marco_disponible;
}


//LA VAMOS A USAR CUANDO USEMOS BUSCAR PAGINA LIBRE
int aplicarLRU(){
	//dentro va a estar el journaling

	//declaro el time, la pag, el segmento y la pos de la pag en tp del seg debido a que los asignare a futuro para la operatoria
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
				seg_con_pag_mas_vieja = segmento_a_evaluar;
				pos_pag_en_tp_seg = j;
				hay_pag_a_liberar = true;

			}
		}
	}

	//si salgo del for y no hay pagina a liberar significa que todas las paginas de todos los segmentos se encuentran modificados (FULL) o que algo raro pasó
	if(!hay_pag_a_liberar){

		printf("Aplico Journal debido a que mi memoria se encuentra FULL\n");
		logInfo("[estructurasMemoria]: se aplicara el Journal debido a que mi memoria se encuentra FULL");

		journalReq();
		se_hizo_journal = true;

		return 0;
	}
	else{
		//significa que encontre la pag mas vieja y tiene flag == 0
		//devuelvo la misma pagina que pag_mas_vieja

		t_est_pag* pagina_a_remover = (t_est_pag*)list_remove(seg_con_pag_mas_vieja->tabla_paginas.paginas, pos_pag_en_tp_seg);
		estados_memoria[pagina_a_remover->offset] = LIBRE;
		int marco_libre = pagina_a_remover->offset;
		logInfo("Se ha encontrado la página con más tiempo sin ser utilizada y su marco es %d", marco_libre);
		free(pagina_a_remover);

		return marco_libre;
	}
}

void eliminar_segmentos(){

	logInfo("[]: Se procede a eliminar los segmentos con sus respectivas paginas");

	int cant_segmentos = list_size(lista_segmentos);

	for(int i = 0; i<cant_segmentos; i++){

		t_segmento* segmento = list_get(lista_segmentos, i);

		t_list* paginas_segmento = segmento->tabla_paginas.paginas;
		list_destroy_and_destroy_elements(paginas_segmento, &free);
	}

	list_clean_and_destroy_elements(lista_segmentos, &free);
}

void liberar_marcos(){

	logInfo("[estructurasMemoria]: Se procede a liberar los marcos en memoria principal");

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
