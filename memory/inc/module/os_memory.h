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
