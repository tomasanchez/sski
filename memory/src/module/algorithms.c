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

page_table_lvl_2_t **
create_big_table(memory_t *memory, uint32_t table_index);
// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

// TODO: La funcion que llama a los clock tiene que validar que la pagina no se encuentre en memoria.

uint32_t clock_selector(void *self, uint32_t table_index)
{

	static uint32_t clock = 0;

	memory_t *memory = (memory_t *)self;
	size_t rows = memory->max_rows;
	size_t total_rows = rows * rows;
	page_table_lvl_2_t **big_table = create_big_table(memory, table_index);

	// >>>> 5
	// [0]        		3 -> 0
	// [1]   	        ? -> 1
	// [2]      	X	5 -> 0

	uint32_t frame = INVALID_FRAME;
	// Wether a FRAME was matched or not
	bool completed = false;
	// Look for a zero FROM CLOCK
	for (uint32_t j = clock; j < total_rows && !completed; j++)
	{
		// When found
		if (!(big_table[j])->use)
		{
			// Replacement
			frame = (big_table[j])->frame;
			(big_table[j])->use = true;
			clock = j + 1 >= total_rows ? 0 : j + 1;
			completed = true;
		}
		// SET bit U to 0
		else if (j == clock)
		{
			(big_table[clock])->use = false;
		}
	}

	// When nothing matches - Look from after until clock.
	for (uint32_t j = 0; j <= clock && !completed; j++)
	{

		if (!(big_table[j])->use)
		{
			frame = (big_table[j])->frame;
			(big_table[j])->use = true;
			clock = j + 1 >= total_rows ? 0 : j + 1;
			completed = true;
		}
	}

	// When all are in use -> reset them all
	if (!completed)
	{
		for (uint32_t j = 0; j < total_rows && !completed; j++)
		{
			(big_table[j])->use = false;
		}

		frame = (big_table[clock])->frame;
		(big_table[clock])->use = true;
		// Reset clock when cycles
		clock = clock + 1 <= total_rows ? 0 : clock + 1;
	}

	free(big_table);

	return frame;
}

operands_t improved_clock_selector(void *self, uint32_t table_index)
{

	static uint32_t clock = 0;

	memory_t *memory = (memory_t *)self;

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
		for (uint32_t j = clock; j < memory->max_rows; j++)
		{

			// When found return INDEX
			if (!pt_2[j].use && !pt_2[j].modified)
			{
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
		}
		for (uint32_t j = 0; j < clock; j++)
		{

			if (!pt_2[j].use && !pt_2[j].modified)
			{
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

			if (!pt_2[j].use && pt_2[j].modified)
			{
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
			else
				pt_2[j].use = false;
		}
		for (uint32_t j = 0; j < clock; j++)
		{

			if (!pt_2[j].use && !pt_2[j].modified)
			{
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
		}

		// Repito paso 1
		for (uint32_t j = clock; j < memory->max_rows; j++)
		{

			// When found return INDEX
			if (!pt_2[j].use && !pt_2[j].modified)
			{
				index.op1 = i;
				index.op2 = j;
				clock = j + 1 >= memory->max_rows ? 0 : j + 1;
				return index;
			}
		}
		for (uint32_t j = 0; j < clock; j++)
		{

			if (!pt_2[j].use && !pt_2[j].modified)
			{
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

page_table_lvl_2_t **
create_big_table(memory_t *memory, uint32_t table_index)
{
	page_table_lvl_1_t *pt_1 = safe_list_get(memory->tables_lvl_1, table_index);
	size_t total_rows = memory->max_rows * memory->max_rows;

	page_table_lvl_2_t **big_table = malloc(sizeof(page_table_lvl_2_t *) * total_rows);

	for (size_t i = 0, k = 0; i < memory->max_rows && k < total_rows; i++)
	{
		page_table_lvl_2_t *pt_2 = safe_list_get(memory->tables_lvl_2, pt_1[i].second_page);

		for (size_t j = 0; j < memory->max_rows; j++, k++)
		{
			big_table[k] = &pt_2[j];
		}
	}

	return big_table;
}
