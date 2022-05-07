/**
 * @file routines.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar> & Luciano Schirripa <lschirripa@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-07-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

/**
 * @brief Maps the different Request of a server.
 *
 * @param fd the request-sender's file descriptor (socket)
 * @return null ptr
 */
void *request_handler(void *fd);
