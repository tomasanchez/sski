/**
 * @file kernel_controller.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

/**
 * @brief Construct a new kernel controller swap object
 *
 * @param socket
 */
void kernel_controller_swap(int socket);

void kernel_controller_read_swap(int socket);

void kernel_controller_memory_init(int socket);

void kernel_controller_delete_swap_file(int socket);