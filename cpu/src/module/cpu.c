/**
 * @file cpu.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-23-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "lib.h"
#include "cpu.h"
#include "log.h"
#include "cfg.h"
#include <signal.h>

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

cpu_t g_cpu;

static void handle_sigint(int signal)
{
	if (signal == SIGINT)
	{
		LOG_WARNING("SIGNINT was received.");
		on_before_exit(&g_cpu);
		exit(SIGINT);
	}
}

/**
 * @brief Initializes the CPU reference.
 *
 * @param cpu the module reference
 * @return the return code.
 */
static int
on_cpu_init(cpu_t *cpu);

/**
 * @brief Frees the CPU memory usage.
 *
 * @param cpu to be deleted
 * @return the exit status.
 */
static int
on_cpu_destroy(cpu_t *cpu);

static int on_cpu_init(cpu_t *cpu)
{

	cpu->pcb = NULL;
	cpu->tm = new_thread_manager();

	// TODO: Add server for Kernel - For PCB.
	// TODO: Add server for Kernel - For Interruptions
	// TODO: Add client connection to Memory.
	return EXIT_SUCCESS;
}

static int
on_cpu_destroy(cpu_t *cpu)
{
	pcb_destroy(cpu->pcb);
	thread_manager_destroy(&cpu->tm);
	// TODO: Add server for Kernel - For PCB.
	// TODO: Add server for Kernel - For Interruptions
	// TODO: Add client connection to Memory.
	return EXIT_SUCCESS;
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

// ------------------------------------------------------------
//  Life Cycle
// ------------------------------------------------------------

int on_init(cpu_t *cpu)
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

	// Attach del evento de interrupcion forzada.
	signal(SIGINT, handle_sigint);

	on_cpu_init(cpu);
	LOG_DEBUG("Module started SUCCESSFULLY");
	return EXIT_SUCCESS;
}

int on_run(cpu_t *cpu)
{
	LOG_TRACE("Hello World!");

	return EXIT_SUCCESS;
}

int on_before_exit(cpu_t *cpu)
{
	int exit_code = EXIT_SUCCESS;

	LOG_WARNING("Closing Module...");

	config_close();
	LOG_WARNING("Configurations unloaded.");

	on_cpu_destroy(cpu);
	LOG_WARNING("CPU has been shut down.");

	LOG_TRACE("Program ended.");
	log_close();

	return exit_code;
}

// ------------------------------------------------------------
//  Event Handlers
// ------------------------------------------------------------