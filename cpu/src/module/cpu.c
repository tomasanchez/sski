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

#include "conexion_memoria.h"
#include "../../include/server/routines.h"
#include "cpu.h"
#include "lib.h"
#include "cpu.h"
#include "log.h"
#include "cfg.h"
#include "conexion.h"
#include "accion.h"
#include "instruction.h"

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
	cpu->server_dispatch = servidor_create(ip(), puerto_escucha());
	cpu->server_interrupt = servidor_create(ip(), puerto_escucha());

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
	servidor_destroy(&(cpu->server_dispatch));
	// TODO: Add server for Kernel - For Interruptions
	servidor_destroy(&(cpu->server_interrupt));
	// TODO: Add client connection to Memory.
	return EXIT_SUCCESS;
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
// !                                  ***** Private Declarations *****
// ============================================================================================================


/**
 * @brief Uses a server to handle CPU dispatch connections.
 *
 * @param cpu the cpu itself
 */
static void
handle_cpu_dispatch(cpu_t *cpu);

/**
 * @brief Uses a server to handle CPU interrupt connections.
 *
 * @param cpu the cpu itself
 */
static void
handle_cpu_interrupt(cpu_t *cpu);


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

	// TODO: create thread for server-dispatch
	// TODO: create thread for server-interrupt
	LOG_TRACE("Handling CPU dispatch...")
	handle_cpu_dispatch(cpu);
	LOG_DEBUG("CPU dispatch: Ok.")

	LOG_TRACE("Handling CPU interrupt...")
	handle_cpu_interrupt(cpu);
	LOG_DEBUG("CPU interrupt: Ok.")


	// TODO: create thread for memory-conection (CLIENT)
	thread_manager_launch(&cpu->tm, routine_conexion_memoria, cpu);

	for (;;)
	{
		LOG_ERROR("Hola Thread 1");
		sleep(TIEMPO_ESPERA);
	}

	// conexion_init(cpu);

	// thread_manager_launch(&cpu->tm, )

	// conexion_enviar_mensaje(cpu->conexion, "Mando un msj");

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

// ============================================================================================================
//                                   ***** Internal Methods  *****
// ============================================================================================================

static void
handle_cpu_dispatch(cpu_t *cpu)
{
	if (servidor_escuchar(&(cpu->server_dispatch)) == -1)
	{
		LOG_ERROR("[CPU dispatch-Server] :=> Server could not listen.");
		return;
	}

	LOG_DEBUG("[CPU dispatch-Server] :=> Server listening. Awaiting for connections.");

	for (;;)
		servidor_run(&(cpu->server_dispatch), routine);
}

static void
handle_cpu_interrupt(cpu_t *cpu)
{
		if (servidor_escuchar(&(cpu->server_interrupt)) == -1)
	{
		LOG_ERROR("[CPU interrupt-Server] :=> Server could not listen.");
		return;
	}

	LOG_DEBUG("[CPU dispatch-Server] :=> Server listening. Awaiting for connections.");

	for (;;)
		servidor_run(&(cpu->server_dispatch), routine);
}
