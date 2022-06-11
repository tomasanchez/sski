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
#include "request_handler.h"
#include "lib.h"
#include "cpu.h"
#include "log.h"
#include "cfg.h"
#include "conexion.h"
#include "accion.h"
#include "instruction.h"
#include "operands.h"
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
	cpu->server_dispatch = servidor_create(ip_memoria(), puerto_escucha_dispatch());
	cpu->server_interrupt = servidor_create(ip_memoria(), puerto_escucha_interrupt());
	sem_init(&(cpu->sem_pcb), SHARE_BETWEEN_THREADS, 0);

	cpu->sync = init_sync();

	return EXIT_SUCCESS;
}

static int
on_cpu_destroy(cpu_t *cpu)
{
	pcb_destroy(cpu->pcb);
	thread_manager_destroy(&cpu->tm);
	servidor_destroy(&(cpu->server_dispatch));
	servidor_destroy(&(cpu->server_interrupt));
	conexion_destroy(&(cpu->conexion));
	sem_destroy(&(cpu->sem_pcb));
	sync_destroy(&(cpu->sync));
	return EXIT_SUCCESS;
}

// ============================================================================================================
// !                                  ***** Private Declarations *****
// ============================================================================================================

/**
 * @brief Runs both CPU servers in different threads.
 *
 * @param cpu the CPU module context object#include "operands.h"
 * @return should be exit success.
 */
static int serve_kernel(cpu_t *data);

/**
 * @brief A procedure for the Dispatch-Server
 *
 * @param sv_data the server itself
 * @return null ptr
 */
static void *
dispatch_server_routine(void *sv_data);

/**
 * @brief A procedure for the Interrupt-Server
 *
 * @param sv_data the server itself
 * @return null ptr
 */
static void *
interrupt_server_routine(void *sv_data);

/**
 * @brief Starts a server logging the server name
 *
 * @param server a server reference
 * @param server_name the server name to be logged
 * @return should return an exit code of succes
 */
static int
on_run_server(servidor_t *server, const char *server_name);

/**
 * @brief
 *
 * @param cpu
 */
void cycle(cpu_t *cpu);

/**
 * @brief
 *
 * @return instruction_t
 */
instruction_t *instructionFetch();

/**
 * @brief
 *
 * @return operands_t
 */
operands_t fetch_operands(cpu_t *cpu);

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

	serve_kernel(cpu);

	routine_conexion_memoria(cpu);

	for (;;)
	{
		// WAIT TO RECEIVE A CPU from a Kernel.
		LOG_TRACE("Waiting for[CPU] :=> Waiting for a process...");
		WAIT(cpu->sync.pcb_received);
		LOG_DEBUG("[CPU] :=> Executing process...");
		cycle(cpu);
	}

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

void cycle(cpu_t *cpu)
{

	instruction_t *instruction;

	instruction = instructionFetch(cpu);

	operands_t operandos;

	if (decode(instruction))
	{
		operandos = fetch_operands(cpu);
	}

	instruction_execute(instruction, 0, 0, NULL);
	// TODO: Execute I/O

	// TODO: Execute EXIT
}

operands_t fetch_operands(cpu_t *cpu)
{

	ssize_t bytes = -1;

	void *send_stream = pcb_to_stream(cpu->pcb);

	conexion_enviar_stream(cpu->conexion, OP, send_stream, pcb_bytes_size(cpu->pcb));

	void *receive_stream = conexion_recibir_stream(cpu->conexion.socket, &bytes);

	operands_t ret = operandos_from_stream(receive_stream);

	free(send_stream);
	free(receive_stream);

	return ret;
}

instruction_t *instructionFetch(cpu_t *cpu)
{
	instruction_t *instruction = list_get(cpu->pcb->instructions, cpu->pcb->pc);

	cpu->pcb->pc++;
	return instruction;
}

bool decode(instruction_t *instruction)
{
	return instruction->icode == C_REQUEST_COPY;
}

// ------------------------------------------------------------
//  Event Handlers
// ------------------------------------------------------------

// ============================================================================================================
//                                   ***** Internal Methods  *****
// ============================================================================================================

static int serve_kernel(cpu_t *cpu)
{

	thread_manager_launch(&cpu->tm, dispatch_server_routine, &cpu->server_dispatch);
	thread_manager_launch(&cpu->tm, interrupt_server_routine, &cpu->server_interrupt);

	return EXIT_SUCCESS;
}

static void *
dispatch_server_routine(void *sv_data)
{
	servidor_t *server = (servidor_t *)sv_data;

	on_run_server(server, "Dispatch-Server");

	return NULL;
}

static void *
interrupt_server_routine(void *sv_data)
{
	servidor_t *server = (servidor_t *)sv_data;

	on_run_server(server, "Interrupt-Server");

	return NULL;
}

static int
on_run_server(servidor_t *server, const char *server_name)
{
	if (servidor_escuchar(server) == -1)
	{
		LOG_ERROR("[CPU:%s] :=> Server could not listen.", server_name);
		return SERVER_RUNTIME_ERROR;
	}

	LOG_DEBUG("[CPU:%s] :=> Server listening. Awaiting for connections.", server_name);

	for (;;)
		servidor_run(server, request_handler);

	return EXIT_SUCCESS;
}

void instruction_execute(instruction_t *instruction, uint32_t param1, uint32_t param2, void *data)
{
	switch (instruction->icode)
	{
	case C_REQUEST_NO_OP:
		execute_NO_OP(retardo_noop());
		break;

	case C_REQUEST_IO:
		execute_IO(data);
		break;

	case C_REQUEST_EXIT:
		execute_EXIT(data);
		break;

	default:
		break;
	}
}

void execute_NO_OP(uint time)
{
	sleep(time);
}

void execute_IO(cpu_t *cpu)
{
	cpu->pcb_result = INOUT;
	SIGNAL(cpu->sem_pcb);
}

void execute_EXIT(cpu_t *cpu)
{
	cpu->pcb_result = PCB;
	SIGNAL(cpu->sem_pcb);
}
