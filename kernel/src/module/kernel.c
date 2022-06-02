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
#include "pcb.h"
#include "pids.h"

// ============================================================================================================
//                                   ***** Init / Destroy Methods  *****
// ============================================================================================================
/**
 * @brief Initializes the Kernel
 *
 * @param kernel the module itself
 * @return success or failure
 */
static int on_init_kernel(kernel_t *kernel);

/**
 * @brief Destroy the kernel elements
 *
 * @param kernel the Kernel
 */
static void on_delete_kernel(kernel_t *kernel);

/**
 * @brief Inits the Kernel Synchronizer
 *
 * @param sync to be initialized
 * @return exit status code
 */
static int on_init_sync(ks_t *sync);

/**
 * @brief Destroys the Kernel Synchronizer
 *
 * @param sync to be destroyed
 */
static void on_destroy_sync(ks_t *sync);

static int on_init_kernel(kernel_t *kernel)
{
	kernel->server = servidor_create(ip(), puerto_escucha());
	kernel->tm = new_thread_manager();
	kernel->pcbs = new_safe_list();
	kernel->pids = new_pids();
	kernel->multiprogramming_grade = grado_multiprogramacion();
	kernel->scheduler = new_scheduler(kernel->multiprogramming_grade);
	on_init_sync(&kernel->sync);
	return EXIT_SUCCESS;
}

static void
on_delete_kernel(kernel_t *kernel)
{
	thread_manager_destroy(&(kernel->tm));
	LOG_TRACE("Thread Manager Ended.");
	on_destroy_sync(&kernel->sync);
	scheduler_delete(kernel->scheduler);
	LOG_TRACE("Syncrhonizer Ended.");
	safe_list_destroy_with(kernel->pcbs, pcb_destroy);
	pids_destroy(&kernel->pids);

	// Destroy CPU Connections
	conexion_destroy(&(kernel->conexion_dispatch));
	conexion_destroy(&(kernel->conexion_interrupt));
	LOG_TRACE("CPU Connections Stoppped.");

	// Destroy Memory Connection
	conexion_destroy(&(kernel->conexion_memory));
	LOG_TRACE("Memory Connection Stopped.");

	servidor_destroy(&(kernel->server));
}

static int on_init_sync(ks_t *sync)
{
	sem_init(&sync->interrupt, SHARE_BETWEEN_THREADS, 0);
	sem_init(&sync->memory, SHARE_BETWEEN_THREADS, 0);
	return EXIT_SUCCESS;
}

static void on_destroy_sync(ks_t *sync)
{
	sem_destroy(&sync->interrupt);
	sem_destroy(&sync->memory);
}

// ============================================================================================================
// !                                  ***** Private Declarations *****
// ============================================================================================================

/**
 * @brief Uses a server to handle console connections.
 *
 * @param kernel the kernel itself
 */
static void
handle_consoles(kernel_t *kernel);

/**
 * @brief Uses two threads to make the different connections to a CPU.
 *
 * @param kernel the kernel itself
 */
static void
handle_cpu(kernel_t *kernel);

/**
 * @brief Uses a dedicated thread to connect to a Memory.
 *
 * @param kernel the kernel itself
 */
static void
handle_memory(kernel_t *kernel);
// ============================================================================================================
// ?                                 ***** Public Functions  *****
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

	if (on_init_kernel(kernel) EQ EXIT_SUCCESS)
	{
		LOG_DEBUG("kernel initializated");
	}

	signals_init();

	LOG_DEBUG("Server created at %s:%s", ip(), puerto_escucha());

	LOG_DEBUG("Kernel Module started SUCCESSFULLY");

	return EXIT_SUCCESS;
}

int on_run(kernel_t *kernel)
{
	LOG_TRACE("Handling Memory...");
	handle_memory(kernel);
	LOG_DEBUG("Memory: Ok.")

	LOG_TRACE("Handling CPU...")
	handle_cpu(kernel);
	LOG_DEBUG("CPU: Ok.")

	LOG_TRACE("Handling Consoles...");
	handle_consoles(kernel);

	return EXIT_SUCCESS;
}

void on_before_exit(kernel_t *kernel, int exit_code)
{
	LOG_WARNING("Closing Kernel...");

	on_delete_kernel(kernel);

	LOG_WARNING("Server has stopped.");

	config_close();

	LOG_WARNING("Configurations unloaded.");
	LOG_TRACE("Kernel ended with status <%d>.", exit_code);
	log_close();

	exit(exit_code);
}

// ============================================================================================================
//                                   ***** Internal Methods  *****
// ============================================================================================================

static void
handle_consoles(kernel_t *kernel)
{
	// Console Connection:
	if (servidor_escuchar(&(kernel->server)) == -1)
	{
		LOG_ERROR("[Console-Server] :=> Server could not listen.");
		return;
	}

	LOG_DEBUG("[Console-Server] :=> Server listening. Awaiting for connections.");

	for (;;)
		servidor_run(&(kernel->server), routine);
}

static void
handle_cpu(kernel_t *kernel)
{
	// CPU Connections:
	thread_manager_launch(&kernel->tm, routine_conexion_dispatch, kernel);
	thread_manager_launch(&kernel->tm, routine_conexion_interrupt, kernel);
}

static void
handle_memory(kernel_t *kernel)
{
	// Memory Connection:
	thread_manager_launch(&(kernel->tm), routine_conexion_memoria, kernel);
}
