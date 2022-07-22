/**
 * @file fs.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief File System implementation
 * @version 0.1
 * @date 07-22-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "lib.h"

void create_file(uint32_t pid);

/**
 * @brief Opens a File descriptor for a process swap.
 *
 * @param pid the process id.
 * @return the file descriptor number
 */
int open_file(uint32_t pid);
