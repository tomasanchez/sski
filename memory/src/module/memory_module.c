/**
 * @file memory.c
 * @author fmaver@frba.utn.edu.ar
 * @brief
 * @version 0.1
 * @date 2022-04-28
 *
 * @copyright Copyright (c) 2022
 *
 */

// ============================================================================================================
//                                   ***** Dependencies  *****
// ============================================================================================================

#define MODULE_NAME "memory"

#include "lib.h"
#include "log.h"
#include "cfg.h"
#include "thread_manager.h"
#include "memory_module.h"
#include "memory_routines.h"
#include "signals.h"

// ============================================================================================================
//                                   ***** Private Functions  *****
// ============================================================================================================
/**
 * @brief Initializes the Memory Memory
 *
 * @param memory the Memory memory
 * @return success or failure
 */
static int on_init_memory(memory_t *memory);

/**
 * @brief Destroy the memory elements
 *
 * @param memory the Memory Memory
 */
static void on_delete_memory(memory_t *memory);

static int on_init_memory(memory_t *memory)
{
	// memory->server = servidor_create(ip(), puerto_escucha());
	memory->server = servidor_create("127.0.0.1", puerto_escucha());

	// TODO : Init Kernel Connection

	// TODO: Init Memory Connection
	memory->tm = new_thread_manager();

	return EXIT_SUCCESS;
}

static void
on_delete_memory(memory_t *memory)
{
	servidor_destroy(&(memory->server));

	// TODO : Destroy Kernel Connection

	// TODO: Destroy Memory Connection
	thread_manager_destroy(&(memory->tm));
}

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

int on_init(memory_t *memory)
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

	if (on_init_memory(memory) EQ EXIT_SUCCESS)
	{
		LOG_DEBUG("Memory initializated");
	}

	/* BO initialization routines */

	/* EO initialization routines */

	signals_init();

	// LOG_DEBUG("Server created at %s:%s", ip(), puerto_escucha());
	LOG_DEBUG("Server created at %s:%s", "127.0.0.1", puerto_escucha());

	LOG_DEBUG("Memory Module started SUCCESSFULLY");

	return EXIT_SUCCESS;
}

int on_run(memory_t *memory)
{

	if (servidor_escuchar(&(memory->server)) == -1)
	{
		LOG_ERROR("Server could not listen.");
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("Server listenning. Awaiting for connections.");

	for (;;)
		servidor_run(&(memory->server), routine);

	return EXIT_SUCCESS;
}

void on_before_exit(memory_t *memory, int exit_code)
{
	LOG_WARNING("Closing Memory...");

	on_delete_memory(memory);

	LOG_WARNING("Server has stopped.");

	/* BO finalization routines */

	/* EO finalization routines */

	config_close();

	LOG_WARNING("Configurations unloaded.");
	LOG_TRACE("Memory ended with status <%d>.", exit_code);
	log_close();

	exit(exit_code);
}
