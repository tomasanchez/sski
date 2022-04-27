/**
 * @file cpu.h
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
#include "thread_manager.h"
#include "conexion.h"

#define MODULE_NAME "cpu"

/**
 * @brief CPU Module.
 *
 */
typedef struct CPU
{

	// TODO: Add server for Kernel - For PCB.
	// TODO: Add server for Kernel - For Interruptions

	// TODO: Add client connection to Memory.
	// La conexion del cliente
	conexion_t conexion;
	// CPU's Thread Launcher;
	thread_manager_t tm;
	// Current PCB in execution.
	pcb_t *pcb;

} cpu_t;

int on_connect(void *conexion, bool offline_mode);
/**
 * @brief Inits a CPU module.
 *
 * @param cpu to be initialized
 */
int on_init(cpu_t *cpu);

/**
 * @brief Handles the main execution procedure.
 *
 * @param cpu to run
 * @return the exit status code
 */
int on_run(cpu_t *cpu);

/**
 * @brief Frees memory usage of a CPU module.
 *
 * @param cpu the module reference
 * @return an exit staus code
 */
int on_before_exit(cpu_t *cpu);
