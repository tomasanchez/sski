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
 * @brief Envia a cpu la cantidad de entradas por página
 *
 * @param fd
 * @return void
 */
void
cpu_controller_send_entries(int fd);

/**
 * @brief Envia a cpu el tamaño de pagina
 *
 * @param fd
 * @return void
 */
void
cpu_controller_send_size(int fd);

/**
 * @brief Envia a cpu el frame solicitado
 *
 * @param fd
 */
void
cpu_controller_send_frame(int fd);

/**
 * @briefEnvia la pagina de segundo nivel solicitada
 *
 * @param fd
 */
cpu_controller_send_page_second_level(int fd);
