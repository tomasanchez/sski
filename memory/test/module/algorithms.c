/**
 * @file algorithms.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-20-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "ctest.h"
#include "os_memory.h"
#include "lib.h"
#include "memory_module.h"
#include "page_table.h"

#define ROWS 10

void fill_table(page_table_lvl_2_t *table, uint32_t *starting_frame_number)
{
	for (int i = 0; i < ROWS; i++)
	{
		table[i].frame = (*(starting_frame_number))++;
	}
}

CTEST(algorithm, does_fill_table)
{
	page_table_lvl_2_t *original = new_page_table_lvl2(ROWS);
	uint32_t free_frame = 0;
	fill_table(original, &free_frame);
	ASSERT_EQUAL(free_frame, ROWS);
	free(original);
}

CTEST(algorithm, does_have_effect)
{
	page_table_lvl_2_t *original = new_page_table_lvl2(ROWS);
	uint32_t free_frame = 0;
	fill_table(original, &free_frame);
	page_table_lvl_2_t *another = new_page_table_lvl2(ROWS);
	fill_table(another, &free_frame);

	page_table_lvl_2_t **big = malloc(sizeof(page_table_lvl_2_t *) * ROWS * 2);

	for (int i = 0; i < ROWS; i++)
	{
		big[i] = &original[i];
	}
	for (int i = 0; i < ROWS; i++)
	{
		big[ROWS + i] = &another[i];
	}

	(big[0])->present = true;
	ASSERT_TRUE((big[0])->present);
	ASSERT_TRUE(original[0].present);

	free(original);
	free(another);
	free(big);
}
