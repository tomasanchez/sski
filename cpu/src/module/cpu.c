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

static int conexion_init(cpu_t* cpu)
{
	char *port = puerto_kernel();
	char *ip = ip_kernel();

	LOG_DEBUG("Connecting <Cpu> at %s:%s", ip, port);
	cpu->conexion = conexion_cliente_create(ip, port);

	// Test connection with Kernel
	//cpu->conexion = conexion_cliente_create("127.0.0.1", "8000");

	if (on_connect(&cpu->conexion, false) EQ SUCCESS)
	{
		LOG_DEBUG("Connected as CLIENT at %s:%s", ip, port);
		// Test connection with Kernel
		//LOG_DEBUG("Connected as CLIENT at %s:%s", "127.0.0.1", "8000");
	}

	return SUCCESS;
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

	// TODO: create thread for memory-conection (CLIENT)
	thread_manager_launch(&cpu->tm, routine_conexion_memoria, cpu);



	conexion_init(cpu);

	//thread_manager_launch(&cpu->tm, )

	//conexion_enviar_mensaje(cpu->conexion, "Mando un msj");


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

ssize_t on_send_action(conexion_t is_conexion, actioncode_t actioncode, uint32_t param)
{
	accion_t *accion = accion_create(actioncode, param);

	void *stream = accion_serializar(accion);

	ssize_t bytes_sent = conexion_enviar_stream(is_conexion, SYS, stream, sizeof(accion_t));

	free(stream);

	accion_destroy(accion);

	return bytes_sent;
}

ssize_t on_send_instruction(void *conexion, instruction_t *inst)
{
	if (inst)
	{
		// Si envio algo sera mayor a 0
		ssize_t result = instruction_send(*(conexion_t *)conexion, inst) > 0 ? SUCCESS : ERROR;

		free(inst);

		return result;
	}

	return (ssize_t)ERROR;
}
