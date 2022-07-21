/**
 * @file os_memory.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-11-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "memory_module.h"

/**
 * @brief Create a new process object
 *
 * @param memory the Memory Module Instance
 * @return a Level I Table ID
 */
uint32_t create_new_process(memory_t *memory);

/**
 * @brief Deletes a Process Memory
 *
 * @param memory the Memory Module Instance
 * @param table_1  a Level I Table ID
 */
void delete_process(memory_t *memory, uint32_t table_1);

/**
 * @brief Finds the next free frame in Memory
 *
 * @param memory the Memory Module Instance
 * @return An ID or UINT32_MAX if error.
 */
uint32_t find_free_frame(memory_t *memory);

/**
 * @brief Create a frame for table object
 *
 * @param memory the Memory Module Instance
 * @param index	the index of the table
 * @param frame the frame to create
 * @return the index at wich was created
 */
uint32_t create_frame_for_table(memory_t *memory, uint32_t index, uint32_t frame);

/**
 * @brief Writes in the Main Memory
 *
 * @param memory the Memory Module Instance
 * @param physical_address the Physical address offset, to write in
 * @param value to be stored
 * @return uint32_t
 */
uint32_t *write_in_memory(memory_t *memory, uint32_t physical_address, uint32_t value);

/**
 * @brief Reads from Main Memory
 *
 * @param memory the Memory Module Instance
 * @param physical_address the Physical address  offset to read from
 * @return the stored value
 */
uint32_t read_from_memory(memory_t *memory, uint32_t physical_address);

uint32_t
get_table_lvl2_number(memory_t *memory, uint32_t frame);
