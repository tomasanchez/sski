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

uint32_t clock_selector(void *self, uint32_t table_index)
{
	LOG_TRACE("[Memory] Selecting frame to be replaced using CLOCK algorithm");
	// Clock Selector
	static uint32_t clock = 0;

	memory_t *memory = (memory_t *)self;
	size_t rows = memory->max_rows;
	// Rows of the BIG TABLE
	size_t total_rows = rows * rows;
	// A table containing all rows of all pages.
	page_table_lvl_2_t **big_table = create_big_table(memory, table_index);
	// Frame to be replaced
	uint32_t frame = INVALID_FRAME;
	// Wether a FRAME was matched or not
	bool completed = false;

	LOG_WARNING("\t---\tTABLE\tBEFORE\tALGORITHM\t---");
	LOG_TABLE(total_rows, big_table);

	LOG_ERROR("{CLOCK}> %d", clock);
	LOG_TRACE("[CLOCK] :=> Looking for a zero...");
	for (uint32_t j = clock; j < total_rows && !completed; j++)
	{

		if ((big_table[j]->present))
		{ // When found
			if (!(big_table[j])->use)
			{
				// Replacement
				frame = (big_table[j])->frame;
				(big_table[j])->use = true;
				clock = j + 1 >= total_rows ? 0 : j + 1;
				completed = true;
				LOG_DEBUG("[CLOCK] :=> Replacement in %d", j);
			}
			// SET bit U to 0
			else if (j == clock)
			{
				(big_table[clock])->use = false;
				LOG_WARNING("[CLOCK] :=> Index %d is in use, but it is the one to be replaced", j);
			}
		}
	}
	LOG_TRACE("[CLOCK]:=> When nothing matches - Look from after until clock");
	for (uint32_t j = 0; j <= clock && !completed; j++)
	{
		if ((big_table[j]->present))
		{
			if (!(big_table[j])->use)
			{
				frame = (big_table[j])->frame;
				(big_table[j])->use = true;
				clock = j + 1 >= total_rows ? 0 : j + 1;
				completed = true;
			}
		}
	}

	if (!completed)
	{
		LOG_TRACE("[CLOCK] :=> All are in use. Resetting...");
		for (uint32_t j = 0; j < total_rows && !completed; j++)
		{
			(big_table[j])->use = false;
		}
		LOG_TABLE(total_rows, big_table);

		frame = (big_table[clock])->frame;
		(big_table[clock])->use = true;
		// Reset clock when cycles
		clock = clock + 1 <= total_rows ? 0 : clock + 1;
	}
	else
	{
		LOG_WARNING("[CLOCK] :=> No reset was needed.");
	}

	free(big_table);

	return frame;
}

uint32_t improved_clock_selector(void *self, uint32_t table_index)
{
	LOG_TRACE("[Memory] Selecting frame to be replaced using CLOCK Improved algorithm");

	// Improved Clock
	static uint32_t clock = 0;

	memory_t *memory = (memory_t *)self;
	// Rows shortcut
	size_t rows = memory->max_rows;
	// Number of rows of the BIG TABLE
	size_t total_rows = rows * rows;
	// A table containing all rows of all pages.
	page_table_lvl_2_t **big_table = create_big_table(memory, table_index);
	// Frame to be replaced
	uint32_t frame = INVALID_FRAME;
	// Wether a FRAME was matched or not
	bool completed = false;

	LOG_WARNING("\t---\tTABLE\tBEFORE\tALGORITHM\t---");
	LOG_TABLE(total_rows, big_table);
	LOG_ERROR("{CLOCK-I}> %d", clock);

	/*
	 * 1. Empezando desde la posición actual del puntero, recorrer la lista de marcos.
	 * Durante el recorrido, dejar el bit de uso (U) intacto.
	 * El primer marco que se encuentre con U = 0 y M = 0 se elige para el reemplazo.
	 */
	LOG_TRACE("[CLOCK-I] :=> Looking for [U=0, M=0]...");
	// for Row in Table if Present
	// or...
	// table.filter(row.present)
	for (uint32_t i = clock; i < total_rows && !completed; i++)
	{
		if ((big_table[i]->present))
		{
			//! When  U=0 && M=0 -> FRAME to be repalced FOUND!
			if (!(big_table[i])->use && !(big_table[i])->modified)
			{
				LOG_WARNING("[CLOCK-I] :=> Match Found");
				LOG_TABLE_HEADER();
				frame = (big_table[i])->frame;
				(big_table[i])->use = true;
				clock = i + 1 >= total_rows ? 0 : i + 1;
				completed = true;
			}
		}
	}
	// Look from zero to clock
	for (uint32_t i = 0; i <= clock && !completed; i++)
	{
		if ((big_table[i]->present))
		{
			//! When  U=0 && M=0 -> FRAME to be repalced FOUND!
			if (!(big_table[i])->use && !(big_table[i])->modified)
			{
				LOG_WARNING("[CLOCK-I] :=> Match Found");
				LOG_TABLE_HEADER();
				LOG_ENTRY(big_table, i);
				frame = (big_table[i])->frame;
				(big_table[i])->use = true;
				clock = i + 1 >= total_rows ? 0 : i + 1;
				completed = true;
			}
		}
	}

	/*
	 * 2. Si el paso 1 falla, recorrer nuevamente, buscando un marco con U = 0 y M = 1.
	 * El primer marco que cumpla la condición es seleccionado para el reemplazo.
	 * Durante este recorrido, cambiar el bit de uso a 0 de todos los marcos que no se elijan.
	 */
	if (!completed)
	{
		LOG_TRACE("[CLOCK-I] :=> Looking for a  [U=0, M=1]...");
	}
	for (uint32_t i = clock; i < total_rows && !completed; i++)
	{
		if ((big_table[i]->present))
		{
			//! When  U=0 && M=1 -> FRAME to be repalced FOUND!
			if (!(big_table[i])->use && (big_table[i])->modified)
			{
				LOG_WARNING("[CLOCK-I] :=> Match Found");
				LOG_TABLE_HEADER();
				LOG_ENTRY(big_table, i);
				frame = (big_table[i])->frame;
				(big_table[i])->use = true;
				clock = i + 1 >= total_rows ? 0 : i + 1;
				completed = true;
			}
			else
				(big_table[i])->use = false;
		}
	}
	// Look from zero to clock
	for (uint32_t i = 0; i <= clock && !completed; i++)
	{
		if ((big_table[i]->present))
		{
			//! When  U=0 && M=1 -> FRAME to be repalced FOUND!
			if (!(big_table[i])->use && (big_table[i])->modified)
			{
				frame = (big_table[i])->frame;
				(big_table[i])->use = true;
				clock = i + 1 >= total_rows ? 0 : i + 1;
				completed = true;
			}
			// ? Set U=0
			else
				(big_table[i])->use = false;
		}
	}

	/*
	 * 3. Repetir el paso 1.
	 * Ahora uno si o si matcheara, ya que si estaban en U=1 se pasaron a U=0.
	 * El primer marco que se encuentre con U = 0 y M = 0 se elige para el reemplazo.
	 */
	// for Row in Table if Present
	// or...
	// table.filter(row.present)
	if (!completed)
	{
		LOG_TRACE("[CLOCK-I] :=> Looking for [U=0, M=0] (AGAIN)...");
	}
	for (uint32_t i = clock; i < total_rows && !completed; i++)
	{
		if ((big_table[i]->present))
		{
			//! When  U=0 && M=0 -> FRAME to be repalced FOUND!
			if (!(big_table[i])->use && !(big_table[i])->modified)
			{
				frame = (big_table[i])->frame;
				(big_table[i])->use = true;
				clock = i + 1 >= total_rows ? 0 : i + 1;
				completed = true;
			}
		}
	}
	// Look from zero to clock
	for (uint32_t i = 0; i <= clock && !completed; i++)
	{
		if ((big_table[i]->present))
		{
			//! When  U=0 && M=0 -> FRAME to be repalced FOUND!
			if (!(big_table[i])->use && !(big_table[i])->modified)
			{
				frame = (big_table[i])->frame;
				(big_table[i])->use = true;
				clock = i + 1 >= total_rows ? 0 : i + 1;
				completed = true;
			}
		}
	}

	free(big_table);
	return frame;
}

// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================
