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
#include "memory_module.h"
#include "page_table.h"


// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

operands_t clock_selector(void *self, uint32_t table_index){

	static uint32_t clock = 0;

	memory_t *memory = (memory_t*)self;

	page_table_lvl_1_t *pt_1 = safe_list_get(memory->tables_lvl_1, table_index);

	operands_t index = {UINT32_MAX, UINT32_MAX};

	// Iterate over LVL 1 Talbe
	for (uint32_t i = 0; i < memory->max_rows; i++)
	{

		uint32_t talbe2_index = pt_1[i].second_page;
		page_table_lvl_2_t *pt_2 = safe_list_get(memory->tables_lvl_2, talbe2_index);

		// >>>> 9
		//   -->   5 -> 0
		//	  7 -> 1
		//    9 -> 1

		// Look for a zero FROM CLOCK
		for(uint32_t j = clock; j < memory->max_rows  ; j++){

			// When found return INDEX
			if(!pt_2[j].use){
				index.op1 = i;
				index.op2 = j;
				return index;
			}

		}

		// When nothing matches - Look from after until clock.
		for (uint32_t j = 0; j < clock; j++){

			if(!pt_2[j].use){
				index.op1 = i;
				index.op2 = j;
				return index;
			}
		}

		// Case when all entries are in use
		for (uint32_t j = 0; j < memory->max_rows; j++)
		{
			pt_2[j].use = false;
		}

		index.op1 = i;
		index.op2 = clock;

		// Reset clock when cycles
		if(clock + 1 < memory->max_rows)
			clock = 0;
		else
			clock++;

		return index;

	}

	return index;
}


// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================


/**


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

*/
