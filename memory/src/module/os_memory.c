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
#include "log.h"
#include "swap.h"

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
uint32_t *create_lvl2_tables(memory_t *memory, uint32_t rows);

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
uint32_t find_id(safe_list_t *safe_list, bool should_replace);

/**
 * @brief Deletes a Process Related Tables
 *
 * @param memory the Memory module containing the tables
 * @param table_id the LVL 1 Table ID to delete
 */
void delete_related_tables(memory_t *memory, uint32_t table_id);

/**
 * @brief Deletes a LVL 2 Table of Pages
 *
 * @param memory the Memory module containing the tables
 * @param table_id the LVL 2 Table ID to delete
 */
void delete_level_2_table(memory_t *memory, uint32_t table_id);

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

uint32_t create_new_process(memory_t *memory)
{
	return create_table(memory, memory->max_rows, create_lvl2_tables(memory, memory->max_rows));
}

void delete_process(memory_t *memory, uint32_t table_id)
{
	delete_related_tables(memory, table_id);
}

uint32_t create_frame_for_table(memory_t *memory, uint32_t table_index, uint32_t frame)
{
	page_table_lvl_2_t *table = safe_list_get(memory->tables_lvl_2, table_index);

	for (uint32_t i = 0; i < memory->max_rows; i++)
	{
		if (table[i].frame == INVALID_FRAME)
		{
			table[i].present = true;
			table[i].frame = frame;
			table[i].use = true;
			return i;
		}
	}

	return UINT32_MAX;
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

uint32_t *write_in_memory(memory_t *memory, uint32_t physical_address, uint32_t value)
{
	uint32_t wait_time = (uint32_t)retardo_memoria() * 1000;
	usleep(wait_time);
	memcpy(memory->main_memory + physical_address, &value, sizeof(value));
	return (uint32_t *)memory->main_memory + physical_address;
}

uint32_t read_from_memory(memory_t *memory, uint32_t physical_address)
{
	uint32_t value = 0;
	uint32_t wait_time = (uint32_t)retardo_memoria() * 1000;
	usleep(wait_time);
	memcpy(&value, memory->main_memory + physical_address, sizeof(value));
	return value;
}

uint32_t
get_table_lvl2_number(memory_t *memory, uint32_t frame)
{
	uint32_t size = (uint32_t)list_size(memory->tables_lvl_2->_list);

	for (uint32_t i = 0; i < size; i++)
	{
		page_table_lvl_2_t *table = safe_list_get(memory->tables_lvl_2, i);

		if (table)
			for (uint32_t j = 0; j < memory->max_rows; j++)
				if (table[j].frame == frame)
					return i;
	}

	return INVALID_FRAME;
}

page_table_lvl_2_t *
get_frame_ref(memory_t *memory, uint32_t frame)
{
	uint32_t id = get_table_lvl2_number(memory, frame);
	page_table_lvl_2_t *table = safe_list_get(memory->tables_lvl_2, id);

	for (uint32_t i = 0; i < memory->max_rows; i++)
	{
		if (table[i].frame == frame)
		{
			return &table[i];
		}
	}

	return NULL;
}

uint32_t get_frames_used_size(memory_t *memory, uint32_t table_lvl_1)
{
	page_table_lvl_2_t **big_table = create_big_table(memory, table_lvl_1);
	uint32_t size = 0;

	for (uint32_t i = 0; i < memory->max_rows * memory->max_rows; i++)
	{
		if (big_table[i]->frame != INVALID_FRAME)
		{
			size += tam_pagina() + sizeof(big_table[i]->frame);
		}
	}

	free(big_table);

	return size;
}

void *get_frame_address(memory_t *memory, uint32_t frame)
{
	return memory->main_memory + frame * tam_pagina();
}

uint32_t get_offset_for_pcb(memory_t *memory, uint32_t pid)
{
	swap_data_t *data = get_swap_data_for_pcb(memory, pid);
	return data != NULL ? data->offset : 0;
}

void *get_swap_data_for_pcb(memory_t *memory, uint32_t pid)
{
	for (int i = 0; i < list_size(memory->swap_data->_list); i++)
	{
		swap_data_t *metadata = safe_list_get(memory->swap_data, i);

		if (metadata->pid == pid)
		{
			return metadata;
		}
	}

	return NULL;
}

void delete_swap_data(memory_t *memory, uint32_t pid)
{
	int index = -1;

	for (int i = 0; i < list_size(memory->swap_data->_list); i++)
	{
		swap_data_t *metadata = safe_list_get(memory->swap_data, i);

		if (metadata->pid == pid)
		{
			index = i;
			break;
		}
	}

	if (index >= 0)
	{
		list_remove_and_destroy_element(memory->swap_data->_list, index, free);
	}
}

// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================

uint32_t *create_lvl2_tables(memory_t *memory, uint32_t rows)
{
	uint32_t *ids = NULL;
	uint32_t id = 0;
	page_table_lvl_2_t *table = NULL;
	ids = malloc(rows * sizeof(uint32_t));

	// Number of rows of the Level I Table (Equal to rows of Second Level Table)
	for (uint32_t i = 0; i < rows; i++)
	{
		id = find_id(memory->tables_lvl_2, false);
		table = new_page_table_lvl2(rows);

		if ((uint32_t)safe_list_size(memory->tables_lvl_2) > id)
		{
			safe_list_replace(memory->tables_lvl_2, id, table);
		}
		else
		{
			safe_list_add_in_index(memory->tables_lvl_2, id, table);
		}

		ids[i] = id;
	}

	return ids;
}

uint32_t create_table(memory_t *memory, uint32_t rows, uint32_t *ids)
{
	uint32_t id = find_id(memory->tables_lvl_1, true);
	page_table_lvl_1_t *table = new_page_table(rows);

	for (uint32_t i = 0; i < rows; i++)
	{
		table[i].second_page = ids[i];
	}

	free(ids);

	if ((uint32_t)safe_list_size(memory->tables_lvl_1) > id)
	{
		safe_list_replace(memory->tables_lvl_1, id, table);
	}
	else
	{
		safe_list_add_in_index(memory->tables_lvl_1, id, table);
	}

	return id;
}

uint32_t find_id(safe_list_t *safe_list, bool should_replace)
{
	if (list_is_empty(safe_list->_list))
		return 0;

	if (should_replace)
		for (uint32_t i = 0; i < (uint32_t)list_size(safe_list->_list); i++)
		{
			page_table_lvl_1_t *table = safe_list_get(safe_list, i);

			if (table == NULL)
				return i;
		}

	return list_size(safe_list->_list);
}

void delete_related_tables(memory_t *memory, uint32_t table_id)
{
	page_table_lvl_1_t *table = list_get(memory->tables_lvl_1->_list, table_id);

	if (table == NULL)
	{
		LOG_ERROR("Table #%d not found", table_id);
		return;
	}

	LOG_WARNING("\t-\tDeleting\tTable\t#%d\t-\t", table_id);
	print_table(memory, table_id);

	// Iterate over a LVL 1 Table
	for (uint32_t i = 0; i < memory->max_rows; i++)
	{
		// Delete all LVL 2 Tables Associated with the LVL 1 Table
		LOG_WARNING("[Memory] :=> Deleting Table#%d[%d]", table_id, table[i].second_page);
		delete_level_2_table(memory, table[i].second_page);
		LOG_TRACE("[Memory] :=> Table#%d[%d] Deleted", table_id, table[i].second_page);
	}

	// Replace with NULL in the list.
	free(table);
	list_replace(memory->tables_lvl_1->_list, table_id, NULL);
}

void delete_level_2_table(memory_t *memory, uint32_t table_id)
{
	// Destroy a Second page
	page_table_lvl_2_t *lvl2_table = list_get(memory->tables_lvl_2->_list, table_id);

	LOG_TRACE("[Memory] :=> Deleting Table#%d (LVL2)", table_id);

	// Iterate over a LVL 2 Table
	for (uint32_t j = 0; j < memory->max_rows && j < memory->max_frames; j++)
	{
		// Delete all Frames associated with the LVL 2 Table
		if (lvl2_table[j].frame != INVALID_FRAME)
		{
			delete_frame(memory, lvl2_table[j].frame);
			LOG_TRACE("[Memory] :=> Deleted Row[%d]= Frame#%d", j, lvl2_table[j].frame);
		}
	}

	free(lvl2_table);
	list_replace(memory->tables_lvl_2->_list, table_id, NULL);
}

void delete_frame(memory_t *memory, uint32_t id)
{
	if (id <= memory->no_of_frames)
		memory->frames[id] = false;
}
