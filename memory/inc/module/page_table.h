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
#include "lib.h"

/**
 * @brief A Page Table of Level I - References other Tables of Lvl II
 *
 */
typedef struct PageTableLVL1
{
	// ID of the index of the second page table.
	uint32_t second_page;
} page_table_t;

/**
 * @brief A page table of Level II - References frames wich represent a portion of memory
 *
 */
typedef struct PageTableLVL2
{
	// Position of frame
	uint32_t frame;
	// Wether the frame is absent or not
	bool absent;
	// Wether it was used
	bool use;
	// Wether it was modified
	bool modified;
} page_table_lvl_2_t;
