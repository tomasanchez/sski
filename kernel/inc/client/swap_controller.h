/**
 * @file swap_controller.h
 * @author Franco Parente (fparente14@frba.utn.edu.ar)
 * @brief Swap entre Memoria y Disco
 * @version 0.1
 * @date 2022-07-15
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "conexion.h"
#include "pcb.h"

/**
 * @brief Sends a PCB to a dispatch connection
 *
 * @param opcode the operation code
 * @param pcb the process control block
 * @return the number of bytes sent
 */
ssize_t swap_controller_send_pcb(opcode_t opcode, pcb_t *pcb);

/**
 * @brief Request a PCB ID
 *
 * @param pid the Process ID
 * @return the number of bytes sent
 */
ssize_t swap_controller_request_pcb(uint32_t pid);

/**
 * @brief Receives a PCB stream
 *
 * @return a PCB object
 */
void *swap_controller_receive_pcb(void);

/**
 * @brief Sends PID and page_table to memory
 *
 */
ssize_t swap_controller_exit(pcb_t *pcb);

