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


uint32_t (*funcion_seleccion)(page_table_lvl_1_t*);
// funcion


uint32_t seleccionDeMarco(page_table_lvl_1_t* tabla_proceso){

	uint32_t frame_per_process = marcos_por_proceso();
	uint32_t marco;


	if ((tabla_proceso->frame_per_process == marcos_por_proceso) || (!ram_quedanMarcos())){
		marco = funcion_seleccion(tabla_proceso);
	} else {
		marco = ram_obtenerMarco();
	}

	return marco;
}

uint32_t seleccionClock(page_table_lvl_1_t* tabla)
{
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
