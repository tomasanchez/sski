/**
 * @file pcb_controller.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-23-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "pcb.h"
#include "server.h"

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

/**
 * @brief Receivesa a PCB stream.
 *
 * @param fd the file descriptor
 * @return a new PCB instance
 */
pcb_t *receive_pcb(int fd);
