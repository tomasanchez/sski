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
#include "main.h"

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
	// TODO : Init CPU Connection
	// TODO: Init Memory Connection
	context->tm = new_thread_manager();

	return EXIT_SUCCESS;
}

static void
on_delete_context(context_t *context)
{
	servidor_destroy(&(context->server));
	// TODO : Destroy CPU Connection
	// TODO: Destroy Memory Connection
	thread_manager_destroy(&(context->tm));
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

	// TODO: use different threads for each connection.

	if (servidor_escuchar(&(context->server)) == -1)
	{
		LOG_ERROR("Server could not listen.");
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("Server listening. Awaiting for connections.");

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
