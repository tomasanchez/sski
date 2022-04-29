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
 * @brief Initializes the Kernel
 *
 * @param kernel the module itself
 * @return success or failure
 */
static int on_init_context(kernel_t *kernel);

/**
 * @brief Destroy the kernel elements
 *
 * @param kernel the Kernel
 */
static void on_delete_context(kernel_t *kernel);

static int on_init_context(kernel_t *kernel)
{
	kernel->server = servidor_create(ip(), puerto_escucha());
	kernel->tm = new_thread_manager();
	return EXIT_SUCCESS;
}

static void
on_delete_context(kernel_t *kernel)
{

	servidor_destroy(&(kernel->server));
	thread_manager_destroy(&(kernel->tm));

	// Destroy CPU Connections
	conexion_destroy(&(kernel->conexion_dispatch));
	conexion_destroy(&(kernel->conexion_interrupt));

	// Destroy Memory Connection
	conexion_destroy(&(kernel->conexion_memory));
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

int on_init(kernel_t *kernel)
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

	if (on_init_context(kernel) EQ EXIT_SUCCESS)
	{
		LOG_DEBUG("kernel initializated");
	}

	/* BO initialization routines */

	/* EO initialization routines */

	signals_init();

	LOG_DEBUG("Server created at %s:%s", ip(), puerto_escucha());

	LOG_DEBUG("Kernel Module started SUCCESSFULLY");

	return EXIT_SUCCESS;
}

int on_run(kernel_t *kernel)
{
	// Dispatch Different threads for each connection

	// CPU Connections:
	thread_manager_launch(&kernel->tm, routine_conexion_dispatch, kernel);
	thread_manager_launch(&kernel->tm, routine_conexion_interrupt, kernel);

	// Memory Connection:
	thread_manager_launch(&(kernel->tm), routine_conexion_memoria, kernel);

	// Console Connection:
	if (servidor_escuchar(&(kernel->server)) == -1)
	{
		LOG_ERROR("Server could not listen.");
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("[SERVER-THREAD] - Server listening. Awaiting for connections.");

	for (;;)
		servidor_run(&(kernel->server), routine);

	return EXIT_SUCCESS;
}

void on_before_exit(kernel_t *kernel, int exit_code)
{
	LOG_WARNING("Closing Kernel...");

	on_delete_context(kernel);

	LOG_WARNING("Server has stopped.");

	/* BO finalization routines */

	/* EO finalization routines */

	config_close();

	LOG_WARNING("Configurations unloaded.");
	LOG_TRACE("Kernel ended with status <%d>.", exit_code);
	log_close();

	exit(exit_code);
}
