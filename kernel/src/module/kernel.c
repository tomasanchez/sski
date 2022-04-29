/**
 * @file kernel.c
 * @author Roberto Savinelli <rsavinelli@frba.utn.edu.ar>
 * @brief Updated version by Tomas Sanchez
 * @version 0.2
 * @date 04-22-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

// ============================================================================================================
//                                   ***** Dependencies  *****
// ============================================================================================================

#include "server.h"
#include "lib.h"
#include "log.h"
#include "cfg.h"
#include "routines.h"
#include "thread_manager.h"
#include "signals.h"
#include "kernel.h"
#include "conexion_memory.h"
#include "main.h"
#include "conexion_dispatch.h"
#include "conexion_interrupt.h"

// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================
/**
 * @brief Initializes the Kernel Context
 *
 * @param context the kernel context
 * @return success or failure
 */
static int on_init_context(context_t *context);

/**
 * @brief Destroy the context elements
 *
 * @param context the Kernel Context
 */
static void on_delete_context(context_t *context);

static int on_init_context(context_t *context)
{
	context->server = servidor_create(ip(), puerto_escucha());
	context->tm = new_thread_manager();

	return EXIT_SUCCESS;
}

static void
on_delete_context(context_t *context)
{
  
	servidor_destroy(&(context->server));
  
	thread_manager_destroy(&(context->tm));
  
  // Destroy CPU Connections
	conexion_destroy(&(context->conexion_dispatch));
	conexion_destroy(&(context->conexion_interrupt));
  
	// Destroy Memory Connection
	conexion_destroy(&(context->conexion_memory));
 
}

int on_connect(void *conexion, bool offline_mode)
{
	if (offline_mode)
	{
		LOG_WARNING("Module working in offline mode.");
		return ERROR;
	}

	while (!conexion_esta_conectada(*(conexion_t *)conexion))
	{
		LOG_TRACE("Connecting...");

		if (conexion_conectar((conexion_t *)conexion) EQ ERROR)
		{
			LOG_ERROR("Could not connect.");
			sleep(TIEMPO_ESPERA);
		}
	}

	return SUCCESS;
}

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

int on_init(context_t *context)
{
	if (log_init(MODULE_NAME, true) EQ ERROR)
		return LOG_INITIALIZATION_ERROR;

	LOG_DEBUG("Logger started.");

	if (config_init(MODULE_NAME) EQ ERROR)
	{
		LOG_ERROR("Could not open Configuration file.");
		log_close();
		return CONFIGURATION_INITIALIZATION_ERROR;
	}

	LOG_DEBUG("Configurations loaded.");

	if (on_init_context(context) EQ EXIT_SUCCESS)
	{
		LOG_DEBUG("Context initializated");
	}

	/* BO initialization routines */

	/* EO initialization routines */

	signals_init();

	LOG_DEBUG("Server created at %s:%s", ip(), puerto_escucha());

	LOG_DEBUG("Kernel Module started SUCCESSFULLY");

	return EXIT_SUCCESS;
}

int on_run(context_t *context)
{
	// Dispatch Different threads for each connection
  
  // CPU Connections:
	thread_manager_launch(&context->tm, routine_conexion_dispatch, context);
	thread_manager_launch(&context->tm, routine_conexion_interrupt, context);
  
  // Memory Connection:
	thread_manager_launch(&(context->tm), routine_conexion_memoria, context);
  
  // Console Connection:
	if (servidor_escuchar(&(context->server)) == -1)
	{
		LOG_ERROR("Server could not listen.");
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("[SERVER-THREAD] - Server listening. Awaiting for connections.");

	for (;;)
		servidor_run(&(context->server), routine);

	return EXIT_SUCCESS;
}

void on_before_exit(context_t *context, int exit_code)
{
	LOG_WARNING("Closing Kernel...");

	on_delete_context(context);

	LOG_WARNING("Server has stopped.");

	/* BO finalization routines */

	/* EO finalization routines */

	config_close();

	LOG_WARNING("Configurations unloaded.");
	LOG_TRACE("Kernel ended with status <%d>.", exit_code);
	log_close();

	exit(exit_code);
}

