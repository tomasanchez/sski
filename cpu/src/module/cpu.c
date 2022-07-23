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

#include <math.h>
#include "conexion_memoria.h"
#include "pcb_controller.h"
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
#include "tlb.h"

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
	cpu->server_dispatch = servidor_create(ip(), puerto_escucha_dispatch());
	cpu->server_interrupt = servidor_create(ip(), puerto_escucha_interrupt());
	cpu->has_interruption = false;
	cpu->sync = init_sync();

	return EXIT_SUCCESS;
}

static int
on_cpu_destroy(cpu_t *cpu)
{
	pcb_destroy(cpu->pcb);
	LOG_DEBUG("PCB destroyed.");
	servidor_destroy(&(cpu->server_dispatch));
	LOG_DEBUG("Server Dispatch destroyed.");
	servidor_destroy(&(cpu->server_interrupt));
	LOG_DEBUG("Server Interrupt destroyed.");
	conexion_destroy(&(cpu->conexion));
	LOG_DEBUG("Server Interrupt destroyed.");
	thread_manager_destroy(&cpu->tm);
	LOG_DEBUG("CPU Thread Manager destroyed.");
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
instruction_t *instruction_fetch();

/**
 * @brief Request instruction's operands value to memory
 *
 * @return a value
 */
uint32_t fetch_operands(uint32_t logical_address);

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
	LOG_TRACE("Configurations loaded %s", ip());

	// Attach del evento de interrupcion forzada.
	signal(SIGINT, handle_sigint);

	on_cpu_init(cpu);
	LOG_DEBUG("Module started SUCCESSFULLY");
	return EXIT_SUCCESS;
}

int on_run(cpu_t *cpu)
{
	serve_kernel(cpu);
	// TODO --> Descomentar cuando esté terminado Memoria.
	routine_conexion_memoria(cpu);

	LOG_DEBUG("Module is OK.");

	for (;;)
	{
		// WAIT TO RECEIVE A CPU from a Kernel.
		LOG_TRACE("[CPU] :=> Waiting for a process...");
		WAIT(cpu->sync.pcb_received);
		LOG_DEBUG("[CPU] :=> Executing PCB<%d>...", cpu->pcb->id);

		while (cpu->pcb != NULL)
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

// ------------------------------------------------------------
//  Execute Cycle
// ------------------------------------------------------------

void cycle(cpu_t *cpu)
{
	// When no int then should execute instruction
	if (!cpu->has_interruption)
	{
		LOG_TRACE("[CPU] :=> Fetching instruction...");
		// The instruction to be executed
		instruction_t *instruction;
		// Fetch
		instruction = instruction_fetch(cpu);

		if (instruction)
		{
			LOG_DEBUG("[CPU] :=> Instruction fetch");
			// Operands to used
			operands_t operandos = {0, 0};

			if (decode(instruction))
			{
				LOG_INFO("[CPU] :=> Fetching operands...");
				uint32_t value = fetch_operands(instruction->param1);
				instruction->param0 = operandos.op1;
				instruction->param1 = value;
			}
			else
			{
				LOG_WARNING("[CPU] :=> No operands to fetch");
			}

			LOG_TRACE("[CPU] :=> Executing instruction...");
			instruction_execute(instruction, cpu);
		}
		else
		{
			LOG_ERROR("No instruction was fetched.");
		}
	}
	// Otherwise must return the PCB as it is
	else
	{
		LOG_ERROR("[CPU] :=> Interruption received.");
		cpu->pcb->status = PCB_READY;
		tlb_reset(&(cpu->tlb));
		return_pcb(cpu->server_dispatch.client, cpu->pcb, 0);
		cpu->has_interruption = false;
	}
}

uint32_t fetch_operands(uint32_t logical_address)
{
	return execute_READ(logical_address);
}

instruction_t *instruction_fetch(cpu_t *cpu)
{
	instruction_t *instruction = list_get(cpu->pcb->instructions, cpu->pcb->pc);

	cpu->pcb->pc++;
	return instruction;
}

bool decode(instruction_t *instruction)
{
	return instruction->icode == C_REQUEST_COPY;
}

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

	LOG_DEBUG("[CPU:%s] :=> Server listening... Awaiting for connections.", server_name);

	for (;;)
		servidor_run(server, request_handler);

	return EXIT_SUCCESS;
}

// ============================================================================================================
//                                   ***** Instructions  *****
// ============================================================================================================

uint32_t instruction_execute(instruction_t *instruction, void *data)
{
	// The result of an instruction executed
	uint32_t return_value = 0;
	uint32_t pid = g_cpu.pcb->id;

	switch (instruction->icode)
	{
	case C_REQUEST_NO_OP:
		execute_NO_OP(retardo_noop());
		LOG_DEBUG("[CPU] :=> Executed instruction: NO_OP");
		break;

	case C_REQUEST_IO:
		execute_IO(instruction, data);
		LOG_WARNING("[CPU] :=> I/O");
		break;

	case C_REQUEST_EXIT:
		execute_EXIT(instruction, data);
		LOG_ERROR("[CPU] :=> Process #%d exited.", pid);
		break;

	case C_REQUEST_READ:;
		uint32_t memory_response_read = execute_READ(instruction->param0);
		LOG_INFO("[CPU] => Executed READ (%d, %d)", instruction->param0, memory_response_read);
		return_value = memory_response_read;
		break;

	case C_REQUEST_WRITE:
		execute_WRITE(instruction->param0, instruction->param1);
		LOG_INFO("[CPU] :=> Executed WRITE (%d, %d)", instruction->param0, instruction->param1);
		break;

	case C_REQUEST_COPY:;
		uint32_t memory_response_write = execute_COPY(instruction->param0, instruction->param1);
		LOG_INFO("[CPU] :=> Executed COPY (<<%d>>, %d)", memory_response_write, instruction->param1);
		return_value = memory_response_write;
		break;

	default:
		LOG_ERROR("[CPU] :=> Unknown instruction code: %d", instruction->icode);
		break;
	}

	return return_value;
}

void execute_NO_OP(uint time)
{
	sleep(time / 1000);
}

void execute_IO(instruction_t *instruction, cpu_t *cpu)
{
	LOG_TRACE("[CPU] :=> Executing IO Instruction...");
	cpu->pcb->status = PCB_BLOCKED;

	tlb_reset(&(cpu->tlb));

	ssize_t bytes_sent = return_pcb(cpu->server_dispatch.client, cpu->pcb, instruction->param0);

	if (bytes_sent > 0)
	{
		LOG_DEBUG("[CPU] :=> PCB sent to Dispatch-Server.");
	}
	else
	{
		LOG_ERROR("[CPU] :=> PCB could not be sent to Dispatch-Server.");
	}
}

void execute_EXIT(instruction_t *instruction, cpu_t *cpu)
{
	if (cpu->pcb)
	{
		cpu->pcb->status = PCB_TERMINATED;

		if (instruction == NULL)
		{
			LOG_WARNING("[CPU] :=> Instruction is NULL")
		}

		tlb_reset(&(cpu->tlb));

		return_pcb(cpu->server_dispatch.client, cpu->pcb, instruction ? instruction->param0 : 0);
	}
	else
	{
		LOG_ERROR("[CPU] :=> PCB to exit is NULL.");
	}
}

uint32_t execute_READ(uint32_t logical_address)
{
	LOG_TRACE("[CPU - MMU - Read] Getting the physical address from the logical address: %d", logical_address);
	uint32_t physical_address = req_physical_address(&g_cpu, logical_address);

	LOG_INFO("[CPU - Read] Physical address calculated: %d", physical_address);

	ssize_t bytes = -1;
	uint32_t return_value = 0;

	// Re-utilizo operands para aparte de enviar la direccion fisica, poder enviar el pcb-id
	// (me evito usar el big-stream pq solo quiero leer)
	operands_t *operands = malloc(sizeof(operands_t));

	operands->op1 = physical_address;
	operands->op2 = g_cpu.pcb->id;

	void *send_stream = operandos_to_stream(operands);

	// Serializo un Stream mas grande, que ahora contendrá:
	// operands (direc. fisica y valor) y uint32_t (pcb->id)
	void *big_stream = malloc(sizeof(uint32_t) + sizeof(operands_t));

	memcpy(big_stream, &g_cpu.pcb->id, sizeof(uint32_t));
	memcpy(big_stream + sizeof(uint32_t), send_stream, sizeof(operands_t));

	// envio a memoria para leer, el operands que contiene la direc fisica y el id del pcb
	conexion_enviar_stream(g_cpu.conexion, RD, big_stream, sizeof(operands_t) + sizeof(uint32_t));

	free(send_stream);

	void *receive_stream = conexion_recibir_stream(g_cpu.conexion.socket, &bytes);

	LOG_TRACE("[CPU - Read] Bytes read: %ld", bytes);

	return_value = *(uint32_t *)receive_stream;

	LOG_INFO("[CPU - Read] Value Read: %d", return_value);

	free(receive_stream);

	return return_value;
}

void execute_WRITE(uint32_t logical_address, uint32_t value)
{
	LOG_TRACE("[CPU - MMU - Write] Getting the physical address from the logical address: %d", logical_address);
	uint32_t physical_address = req_physical_address(&g_cpu, logical_address);

	LOG_INFO("[CPU - Write] Physical address calculated: %d", physical_address);

	operands_t *operands = malloc(sizeof(operands_t));

	operands->op1 = physical_address;
	operands->op2 = value;

	LOG_TRACE("[CPU - Write] Value to write: %d", operands->op2);

	// Serializo un Stream mas grande, que ahora contendrá:
	// operands (direc. fisica y valor) y uint32_t (pcb->id)
	void *big_stream = malloc(sizeof(uint32_t) + sizeof(operands_t));

	memcpy(big_stream, &g_cpu.pcb->id, sizeof(uint32_t));

	void *send_stream = operandos_to_stream(operands);

	memcpy(big_stream + sizeof(uint32_t), send_stream, sizeof(operands_t));

	conexion_enviar_stream(g_cpu.conexion, WT, big_stream, sizeof(operands_t) + sizeof(uint32_t));

	free(send_stream);
	free(operands);
	free(big_stream);
}

uint32_t
execute_COPY(uint32_t param1, uint32_t param2)
{
	execute_WRITE(param1, param2);

	return param2;
}

// ============================================================================================================
//			   							***** MMU - TLB *****
// ============================================================================================================

/*



Direccion Logica --> 001000  111111

Tam memoria: 4096Bytes = 4KB = 2¹²
Tamaño de pag = Tamaño de frame = 64Bytes -> 2⁶ --> 6 bits de offset

Direc Logica:  |001000|111111|-> offset
			   | Pag  |  Off |


Tabla de Paginas 2:

|N° Pag | Frame |
|	0	| 	5	|
|	1	| 	7	|
|   2   |   6   |
|	8	| 	3	|


|N° Pag | N° PAg |
|	0	| 	5	|
|	1	| 	7	|
|   2   |   6   |
|	8	| 	3	|

3 -> 000011

DF:  |000011|111111|


PRoceso B --> Direc Logica: |000010|000011|
							| Pag  | Off  |
							|  2   |      |

Direc Fisica: 6(Decimal) + 000011
6 -> 0110
=> 000110|000011

*/

uint32_t req_physical_address(cpu_t *cpu, uint32_t logical_address)
{
	LOG_WARNING("[MMU] :=> Translating Logical Address <%d>", logical_address);

	// The Page Number of
	uint32_t page_number = get_page_number(logical_address, cpu->page_size);
	uint32_t frame = VALOR_INVALIDO;
	uint32_t second_page;

	/**
	 * @brief
	 *
	 * ----- Test -----------
	 * LOGICAL ADDRES 132
	 * PAGE SIZE 64
	 * Entries in the TLB: 4
	 * Entries per Table: 4
	 *-----------------------
	 *
	 * [entrada_tabla_1er_nivel | entrada_tabla_2do_nivel | desplazamiento]
	 * 0d = 000 | 000 | 0000000 b
	 * Page Number: 0/64 = 0
	 * Index_LVL1 = 0 Number / 4 = 0
	 * Index_LVL2 = 0 Number % 4 = 0
	 * Offset = 0 - 0 * 64 = 0
	 *
	 *
	 * 132d = 10000100b
	 * Page number = 132/64 = 2
	 * Index_LVL1 = 2 / 4 = 0
	 * Index_LVL2 = 2 % 4 = 2
	 * Offset = 132 - 64 * 2 = 132 - 128 = 4
	 *   000|010|000100
	 *
	 * TP - lvl - 1
	 * 0 | 5
	 * TP - lvl2- #5
	 * 0 | 455
	 * 1 | 5676
	 * 2 | 6475
	 * 3 | 657
	 *
	 *
	 * | PAGINA | MARCO
	 * | 0 | 2|
	 *
	 *
	 * #PAGE 2
	 *  - LVL2 = 99
	 * 	- LVL2 = 102
	 *  - LVL2 = 233
	 *  - LVL2 = 344
	 *
	 * #LVL2 99
	 * 	FRAME 20
	 * 	FRAME 90
	 * 	FRAME 100
	 * 	FRAME 399
	 *
	 * 0 => FRAME 3
	 */
	if (!page_in_TLB(cpu->tlb, page_number, &frame))
	{
		LOG_ERROR("[TLB] :=> Page Not Found");
		LOG_WARNING("[MMU] :=> Accessing Memory...");
		second_page = request_table_2_entry(cpu->pcb->page_table, get_entry_lvl_1(page_number, cpu->page_amount_entries));
		frame = request_frame(second_page, get_entry_lvl_2(logical_address, cpu->page_size, cpu->page_amount_entries));
		cpu->tlb->replace(cpu->tlb, page_number, frame);
		LOG_INFO("[TLB] :=> ADDED: [Page: %d| Frame: %d]", page_number, frame);
	}
	else
	{
		LOG_INFO("[TLB] :=> MATCH: [Page: %d| Frame: %d]", page_number, frame);
	}

	uint32_t physical_address = frame * (cpu->page_size) + get_offset(logical_address, cpu->page_size);

	LOG_INFO("[MMU] :=> Logical Address <<%d>> -> Physical Address <<%d>>", logical_address, physical_address);

	return physical_address;
}

uint32_t get_page_number(uint32_t logic_address, uint32_t page_size)
{
	return (uint32_t)floor(logic_address / page_size);
}

uint32_t get_offset(uint32_t direccion_logica, uint32_t page_size)
{
	return direccion_logica - page_size * get_page_number(direccion_logica, page_size);
}

uint32_t get_entry_lvl_1(uint32_t page_number, uint32_t entries_per_table)
{
	return (uint32_t)floor(page_number / entries_per_table);
}

uint32_t get_entry_lvl_2(uint32_t direccion_logica, uint32_t page_size, uint32_t entries_per_table)
{
	return get_page_number(direccion_logica, page_size) % entries_per_table;
}

uint32_t request_table_2_entry(uint32_t id_lvl_1_table, uint32_t row_index)
{
	LOG_TRACE("[MMU] :=> Request Page of Second Table...");

	operands_t *operands = malloc(sizeof(operands_t));

	operands->op1 = id_lvl_1_table;
	operands->op2 = row_index;

	void *send_stream = operandos_to_stream(operands);

	conexion_enviar_stream(g_cpu.conexion, SND_PAGE, send_stream, sizeof(operands_t));

	uint32_t *ret_page_snd_level = connection_receive_value(g_cpu.conexion, sizeof(uint32_t));

	if (ret_page_snd_level == NULL)
	{
		LOG_ERROR("[Memory-Client] :=> page_second_level can't be NULL");
		return VALOR_INVALIDO;
	}
	else
	{
		LOG_DEBUG("[MMU] :=> page_second_level is: %d", *ret_page_snd_level);
	}

	uint32_t ret_page = *ret_page_snd_level;

	free(ret_page_snd_level);
	free(send_stream);
	free(operands);

	return ret_page;
}

uint32_t request_frame(uint32_t tabla_segundo_nivel, uint32_t offset)
{
	// ENVIO DE STREAM

	LOG_TRACE("[MMU] :=> Request Frame value...");

	operands_t *operands = malloc(sizeof(operands_t));

	operands->op1 = tabla_segundo_nivel;
	operands->op2 = offset;

	void *send_stream = operandos_to_stream(operands);

	void *big_stream = malloc(sizeof(uint32_t) + sizeof(operands_t));

	memcpy(big_stream, &g_cpu.pcb->id, sizeof(uint32_t));
	memcpy(big_stream + sizeof(uint32_t), send_stream, sizeof(operands_t));

	conexion_enviar_stream(g_cpu.conexion, FRAME, big_stream, sizeof(operands_t) + sizeof(uint32_t));

	// RECIBO DE UINT32_T

	uint32_t *frame = connection_receive_value(g_cpu.conexion, sizeof(uint32_t));

	if (frame == NULL)
	{
		LOG_ERROR("[Memory-Client] :=> Frame can't be NULL");
		return VALOR_INVALIDO;
	}
	else
	{
		LOG_DEBUG("[MMU] :=> Frame is: %d", *frame);
	}

	uint32_t ret_frame = *frame;

	free(frame);
	free(send_stream);
	free(operands);

	return ret_frame;
}
