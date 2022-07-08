/**
 * @file cpu_controller.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-21-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "sem.h"
#include "conexion.h"
#include "pcb.h"

typedef struct CpuController
{
	pthread_mutex_t cpu_dispatch;
	pthread_mutex_t cpu_interrupt;

} cpu_controller_t;

/**
 * @brief Init CPU Controller
 *
 */
void init_cpu_controller(void);

/**
 * @brief Destroys the CPU Controller
 *
 */
void destroy_cpu_controller(void);

/**
 * @brief Instantiates a CPU cpu_controller
 *
 * @return a Controller.
 */
cpu_controller_t *cpu_controller_on_init(cpu_controller_t *controller);

/**
 * @brief Deletes memory usage of a CPU Controller
 *
 * @param cpu_controller to be deleted
 */
void cpu_controller_destroy(cpu_controller_t *cpu_controller);

/**
 * @brief Sends a PCB to a dispatch connection
 *
 * @param connection_dispatch to sent
 * @param opcode the operation code
 * @param pcb the process control block
 * @return the number of bytes sent
 */
ssize_t cpu_controller_send_pcb(conexion_t connection_dispatch, opcode_t opcode, pcb_t *pcb);

/**
 * @brief Sends an INTERRUPT signal to a interrupt connection.
 *
 * @param connection_interrupt to sent
 * @return the number of bytes sent
 */
ssize_t cpu_controller_send_interrupt(conexion_t connection_interrupt);

/**
 * @brief Receives a PCB stream
 *
 * @param connection_dispatch
 * @param io_time
 * @return void*
 */
void *cpu_controller_receive_pcb(conexion_t connection_dispatch, uint32_t *io_time);
