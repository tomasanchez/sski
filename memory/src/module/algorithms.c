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

#include "log.h"
#include "lib.h"
#include "algorithms.h"
#include "memory_module.h"
#include "page_table.h"


// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

// TODO: La funcion que llama a los clock tiene que validar que la pagina no se encuentre en memoria.

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

		// >>>> 5
		// [0]        	-->	3 -> 1
		// [1]   	        2 -> 1
		// [2]      		5 -> 1

		// Look for a zero FROM CLOCK
		for(uint32_t j = clock; j < memory->max_rows  ; j++){

			// When found return INDEX
			if(!pt_2[j].use){
				index.op1 = i;
				index.op2 = j;
				pt_2[j].use = true;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}

		}
		// When nothing matches - Look from after until clock.
		for (uint32_t j = 0; j < clock; j++){

			if(!pt_2[j].use){
				index.op1 = i;
				index.op2 = j;
				pt_2[j].use = true;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
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
		pt_2[clock].use = true;

		// Reset clock when cycles
		clock = clock + 1 <= memory->max_rows ? 0 : clock + 1;

		return index;

	}

	return index;
}


operands_t improved_clock_selector(void *self, uint32_t table_index){

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

		/**
		 * 1. Empezando desde la posición actual del puntero, recorrer la lista de marcos.
		 *	Durante el recorrido, dejar el bit de uso (U) intacto.
		 * El primer marco que se encuentre con U = 0 y M = 0 se elige para el reemplazo.
		 */
		for(uint32_t j = clock; j < memory->max_rows  ; j++){

			// When found return INDEX
			if(!pt_2[j].use && !pt_2[j].modified){
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}

		}
		for (uint32_t j = 0; j < clock; j++){

			if(!pt_2[j].use && !pt_2[j].modified){
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
		}

		/*
		 * 2. Si el paso 1 falla, recorrer nuevamente, buscando un marco con U = 0 y M = 1.
		 * El primer marco que cumpla la condición es seleccionado para el reemplazo.
		 * Durante este recorrido, cambiar el bit de uso a 0 de todos los marcos que no se elijan.
		 */
		for (uint32_t j = clock; j < memory->max_rows; j++)
		{

			if(!pt_2[j].use && pt_2[j].modified)
			{
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
			else
				pt_2[j].use = false;

		}
		for (uint32_t j = 0; j < clock; j++){

			if(!pt_2[j].use && !pt_2[j].modified){
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
		}


		// Repito paso 1
		for(uint32_t j = clock; j < memory->max_rows  ; j++){

			// When found return INDEX
			if(!pt_2[j].use && !pt_2[j].modified){
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}

		}
		for (uint32_t j = 0; j < clock; j++){

			if(!pt_2[j].use && !pt_2[j].modified){
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
		}

		return index;

	}

	return index;

}
