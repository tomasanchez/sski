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
#include "cfg.h"
#include "log.h"


/**
 * @brief Kernel module context.
 *
 */
typedef struct Context
{
	// The Kernel Multi-thread Server dependency.
	servidor_t server;
	// TODO: Add Kernel-Memory Client dependency
	// TODO: Add Kernel-CPU Client dependency
	conexion_t conexion_dispatch;
	conexion_t conexion_interrupt;
	thread_manager_t tm;
} context_t;

/**
 * @brief Initializes the Kernel Module with the required structures.
 *
 * @param context the server context structure.
 * @return an status code.
 */
int on_init(context_t *context);

/**
 * @brief Responsible of Kernel main processing action.
 *
 * @param context the server context structure.
 * @return an status code.
 */
int on_run(context_t *context);

/**
 * @brief Method called when Kernel does close. Destroys created structures.
 *
 * @param context the current processing context
 * @param exit_code an status code
 */
void on_before_exit(context_t *context, int exit_code);
