/**
 * @file cpu_controller.h
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
 * @brief
 *
 * @param socket
 */
void cpu_controller_read(int socket);

/**
 * @brief
 *
 * @param fd
 * @return void
 */
void
cpu_controller_send_entries(int fd);

/**
 * @brief
 *
 * @param fd
 * @return void
 */
void
cpu_controller_send_size(int fd);
