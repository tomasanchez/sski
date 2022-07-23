/**
 * @file page_table.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-09-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "page_table.h"
#include "memory_module.h"
#include <stdlib.h>

page_table_lvl_1_t *new_page_table(size_t rows)
{
	page_table_lvl_1_t *table = NULL;
	table = malloc(sizeof(page_table_lvl_1_t) * rows);

	// Set the second page ID to invalid
	for (uint32_t i = 0; i < rows; i++)
	{
		table[i].second_page = UINT32_MAX;
	}

	return table;
}

page_table_lvl_2_t *new_page_table_lvl2(size_t rows)
{
	page_table_lvl_2_t *table = NULL;
	table = malloc(sizeof(page_table_lvl_2_t) * rows);

	// Set the FRAME ID to invalid
	for (uint32_t i = 0; i < rows; i++)
	{
		table[i].frame = UINT32_MAX;
		table[i].modified = false;
		table[i].present = false;
		table[i].use = false;
	}

	return table;
}

page_table_lvl_2_t **
create_big_table(void *memory_ref, uint32_t table_index)
{
	memory_t *memory = (memory_t *)memory_ref;

	page_table_lvl_1_t *pt_1 = safe_list_get(memory->tables_lvl_1, table_index);

	if (pt_1 == NULL)
	{
		LOG_ERROR("Page Table %d not found ", table_index);
		return NULL;
	}

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

void print_table(void *memory_ref, uint32_t table_index)
{
	page_table_lvl_2_t **big_table = create_big_table(memory_ref, table_index);
	uint32_t rows = ((memory_t *)memory_ref)->max_rows * ((memory_t *)memory_ref)->max_rows;
	LOG_TABLE(rows, big_table);
	free(big_table);
}
