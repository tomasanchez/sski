/**
 * runtime.h
 *
 * @file runtime interface for the server module
 * @author Tomás A. Sánchez and R. Nicolás Savinelli
 * @since  03.17.2022
 */

#pragma once

#include "server.h"
#include "thread_manager.h"
#include "safe_list.h"
#include "pids.h"
#include "cfg.h"
#include "log.h"
#include "sem.h"

typedef struct KernelSynchronizer
{
	sem_t dispatch;
	sem_t interrupt;
	sem_t memory;
} ks_t;

/**
 * @brief Kernel module.
 *
 */
typedef struct Kernel
{
	// The Kernel Multi-thread Server dependency.
	servidor_t server;
	// Available PID pool
	pids_t pids;
	// Kernel instantiated PCBs.
	safe_list_t *pcbs;
	// Kernel-Memory Client dependency.
	conexion_t conexion_memory;
	// Kernel-CPU (Dispatch) connection dependency.
	conexion_t conexion_dispatch;
	// Kernel-CPU (Interrupt) connection dependency.
	conexion_t conexion_interrupt;
	// Thread Tracker dependency.
	thread_manager_t tm;
	// Kernel Synchronizer dependency.
	ks_t sync;
} kernel_t;

/**
 * @brief Initializes the Kernel Module with the required structures.
 *
 * @param context the server context structure.
 * @return an status code.
 */
int on_init(kernel_t *context);

/**
 * @brief Responsible of Kernel main processing action.
 *
 * @param context the server context structure.
 * @return an status code.
 */
int on_run(kernel_t *context);

/**
 * @brief Method called when Kernel does close. Destroys created structures.
 *
 * @param context the current processing context
 * @param exit_code an status code
 */
void on_before_exit(kernel_t *context, int exit_code);
