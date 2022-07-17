/**
 * @file algorithms.c
 * @author Francisco Maver (fmaver@frba.utn.edu.ar)
 * @brief
 * @version 0.1
 * @date 2022-07-13
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <commons/string.h>
#include <commons/collections/list.h>

#include "memory_dispatcher.h"
#include "accion.h"
#include "instruction.h"
#include "thread_manager.h"
#include "server.h"
#include "smartlist.h"
#include "accion.h"
#include "log.h"
#include "lib.h"
#include "algorithms.h"


typedef struct RegistroPrimerNivel{
	uint32_t nro_registro;
	uint32_t pagina_madre;
	uint32_t valor;
	bool contiene_valor;
} registro_t;

typedef struct Reloj{
	uint32_t indice;
	t_list* registros;
	t_list* bits;
}reloj_t;

typedef struct Swap{
	FILE* archivo_swap;
	char* swap_name;
	t_list* items;
}swap_t;

typedef struct TablaPrimerNivel{
	registro_t* registros;
	reloj_t* clock;
	uint32_t cantidad_marcos_usados;
	swap_t* swap;
}tabla_pn;

typedef struct TablaSegundoNivel{
	registro_t* registros;
	uint32_t tabla_origen;
	bool presente;
}tabla_sn;



uint32_t (*funcion_seleccion)(page_table_lvl_1_t*);
// funcion


uint32_t seleccionDeMarco(tabla_pn* tabla_proceso){
	uint32_t marco;

	if ((tabla_proceso->cantidad_marcos_usados == marcos_por_proceso()) || (!ram_quedanMarcos())){
		marco = funcion_seleccion(tabla_proceso);
	} else {
		marco = ram_obtenerMarco();
	}

	return marco;
}

uint32_t seleccionClock(tabla_pn* tabla){
	uint32_t size = list_size(tabla->clock->registros);
	while(true){
		registro_t* registro = list_get(tabla->clock->registros,tabla->clock->indice);
		if(ram_estaUsado(registro->valor)){
			ram_setMarcoSinUsar(registro->valor);
		} else{
			quitarRegistro(tabla->clock);
			swap_escribirEnSwap(tabla->swap,registro);
			return registro->valor;
		}
		tabla->clock->indice = (tabla->clock->indice + 1) % size;

	}
}

uint32_t seleccionClockM(tabla_pn* tabla){
	reloj_t* reloj = tabla->clock;
	uint32_t size = list_size(reloj->registros);
	uint32_t counter;

	while(true){
		counter = 0;
		reloj->indice = reloj->indice % size;


		while(counter < size){
			registro_t* registro = list_get(reloj->registros,reloj->indice);
			if (!ram_estaUsado(registro->valor) && !ram_estaModificado(registro->valor)){
				quitarRegistro(reloj);
				swap_escribirEnSwap(tabla->swap,registro);
				return registro->valor;
			}
			counter++;
			reloj->indice = (reloj->indice + 1) % size;
		}

		counter = 0;

		while(counter < size){
			registro_t* registro = list_get(reloj->registros,reloj->indice);
			if ((!ram_estaUsado(registro->valor)) && ram_estaModificado(registro->valor)){
				quitarRegistro(reloj);
				swap_escribirEnSwap(tabla->swap,registro);
				return registro->valor;
			} else{
				ram_setMarcoSinUsar(registro->valor);
			}
			counter++;
			reloj->indice = (reloj->indice + 1) % size;
		}
	}
}

