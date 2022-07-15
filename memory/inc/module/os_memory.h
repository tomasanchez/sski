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
 * @brief Finds the next free frame in Memory
 *
 * @param memory the Memory Module Instance
 * @return An ID or UINT32_MAX if error.
 */
uint32_t find_free_frame(memory_t *memory);
