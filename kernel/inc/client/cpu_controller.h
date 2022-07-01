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

ssize_t cpu_controller_send_pcb(conexion_t connection_dispatch, opcode_t opcode, pcb_t *pcb);

ssize_t cpu_controller_send_interrupt(conexion_t connection_interrupt);

void *cpu_controller_receive_pcb(conexion_t connection_dispatch, uint32_t *io_time);
