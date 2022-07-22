/**
 * @file page_table.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-09-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <inttypes.h>
#include "log.h"
#include "lib.h"

#define INVALID_FRAME UINT32_MAX

/**
 * @brief A Page Table of Level I - References other Tables of Lvl II
 *
 */
typedef struct PageTableLVL1
{
	// ID of the index of the second page table.
	uint32_t second_page;
} page_table_lvl_1_t;

/**
 * @brief A page table of Level II - References frames wich represent a portion of memory
 *
 */
typedef struct PageTableLVL2
{
	// Position of frame
	uint32_t frame;
	// Wether the frame is absent or not
	bool present;
	// Wether it was used
	bool use;
	// Wether it was modified
	bool modified;
} page_table_lvl_2_t;

/**
 * @brief Creates a new Page Table of Level I
 *
 * @param rows the entries per table
 * @return a page table reference
 */
page_table_lvl_1_t *new_page_table(size_t rows);

/**
 * @brief Creates a new Page Table of Level II
 *
 * @param rows the entries per table
 * @return a page table reference
 */
page_table_lvl_2_t *new_page_table_lvl2(size_t rows);

/**
 * @brief Creates a combination of all tables of LEVEL II in one
 *
 * @param memory the module
 * @param table_index the table_index
 * @return a joined table
 */
page_table_lvl_2_t **
create_big_table(void *memory, uint32_t table_index);

/**
 * @brief Logs a Row of a BIG Table
 *
 */
#define LOG_ENTRY(table, entry) LOG_INFO("\t%d\t|%d\t|%d\t|%d\t|%d\t|", entry, (table[entry])->frame, (table[entry])->use, (table[entry])->present, (table[entry])->modified)

#define LOG_TABLE_HEADER() LOG_DEBUG("\tINDEX\t|Frame\t|U\t|P\t|M\t|")

/**
 * @brief Logs a Big Table
 *
 */
#define LOG_TABLE(rows, table)              \
	{                                       \
		LOG_TABLE_HEADER();                 \
		for (uint32_t i = 0; i < rows; i++) \
			LOG_ENTRY(table, i);            \
	}
