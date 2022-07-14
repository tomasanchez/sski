/**
 * @file os_memory.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-11-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

// ============================================================================================================
//                                   ***** Dependencies  *****
// ============================================================================================================

#include "os_memory.h"
#include "page_table.h"

// ============================================================================================================
//                                   ***** Declarations  *****
// ============================================================================================================

/**
 * @brief Creates a Lvl II Page Table in Memory
 *
 * @param memory the memory containign the Tables repositories
 * @param rows the number of entries
 * @param frames  the number of frames
 * @return uint32_t
 */
uint32_t *create_lvl2_tables(memory_t *memory, uint32_t rows, uint32_t frames);

/**
 * @brief Creates a Lvl I Page Table in Memory
 *
 * @param memory the memory containign the Tables repositories
 * @param rows the number of entries
 * @param ids  of the tables
 * @return uint32_t
 */
uint32_t create_table(memory_t *memory, uint32_t rows, uint32_t *ids);

/**
 * @brief Finds the next available ID
 *
 * @param list the list of tables
 * @return an Id
 */
uint32_t find_id(safe_list_t *list);
// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

uint32_t create_new_process(memory_t *memory)
{
	return create_table(memory, memory->max_rows, create_lvl2_tables(memory, memory->max_rows, memory->max_frames));
}

uint32_t find_free_frame(memory_t *memory)
{
	for (uint32_t i = 0; i < memory->no_of_frames; i++)
	{
		if (memory->frames[i] == false)
		{
			memory->frames[i] = true;
			return i;
		}
	}

	return UINT32_MAX;
}

// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================

uint32_t *create_lvl2_tables(memory_t *memory, uint32_t rows, uint32_t frames)
{
	uint32_t *ids = NULL;
	uint32_t id = 0;
	page_table_lvl_2_t *table = NULL;
	ids = malloc(rows * sizeof(uint32_t));

	// Number of rows of the Level I Table (Equal to rows of Second Level Table)
	for (uint32_t i = 0; i < rows; i++)
	{
		id = find_id(memory->tables_lvl_2);
		table = new_page_table_lvl2(rows);

		for (uint32_t j = 0; j < rows && j < frames; j++)
		{
			uint32_t frame = find_free_frame(memory);
			table[j].frame = frame;
			table[j].present = frame != UINT32_MAX;
		}

		list_add_in_index(memory->tables_lvl_2->_list, id, table);

		ids[i] = id;
	}

	return ids;
}

uint32_t create_table(memory_t *memory, uint32_t rows, uint32_t *ids)
{
	uint32_t id = find_id(memory->tables_lvl_1);
	page_table_lvl_1_t *table = new_page_table(rows);

	for (uint32_t i = 0; i < rows; i++)
	{
		table[i].second_page = ids[i];
	}

	free(ids);

	list_add_in_index(memory->tables_lvl_1->_list, id, table);

	return id;
}

uint32_t find_id(safe_list_t *safe_list)
{
	if (list_is_empty(safe_list->_list))
		return 0;

	for (uint32_t i = 0; i < (uint32_t)list_size(safe_list->_list); i++)
	{
		page_table_lvl_1_t *table = list_get(safe_list->_list, i);

		if (table == NULL)
			return i;
	}

	return list_size(safe_list->_list);
}
