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
	cpu->server_dispatch = servidor_create(ip_memoria(), puerto_escucha_dispatch());
	cpu->server_interrupt = servidor_create(ip_memoria(), puerto_escucha_interrupt());

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
 * @return operands values
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
	//TODO --> Descomentar cuando esté terminado el kernel.
	//routine_conexion_memoria(cpu);

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
				operandos = fetch_operands(cpu);
				instruction->param0 = operandos.op1;
				instruction->param1 = operandos.op2;
			}
			else
			{
				LOG_INFO("[CPU] :=> No operands to fetch");
			}

			LOG_DEBUG("[CPU] :=> Executing instruction...");
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
		return_pcb(cpu->server_dispatch.client, cpu->pcb, 0);
		cpu->has_interruption = false;
	}
}

operands_t fetch_operands(cpu_t *cpu)
{

	ssize_t bytes = -1;

	// Sends a PCB
	void *send_stream = pcb_to_stream(cpu->pcb);

	// Request to Memory
	conexion_enviar_stream(cpu->conexion, OP, send_stream, pcb_bytes_size(cpu->pcb));

	// Receives an Operand stream
	void *receive_stream = conexion_recibir_stream(cpu->conexion.socket, &bytes);

	// Retrieves operands from stream
	operands_t ret = operandos_from_stream(receive_stream);

	free(send_stream);
	free(receive_stream);

	return ret;
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
		LOG_ERROR("[CPU] :=> Process exited.");
		break;

	case C_REQUEST_READ:;
		uint32_t memory_response_read = execute_READ(instruction->param0);
		LOG_TRACE("[CPU] => Executed READ (%d, %d)", instruction->param0, memory_response_read);
		return_value = memory_response_read;
		break;

	case C_REQUEST_WRITE:
		execute_WRITE(instruction->param0, instruction->param1);
		LOG_INFO("[CPU] :=> Executed WRITE (%d, %d)", instruction->param0, instruction->param1);
		break;

	case C_REQUEST_COPY:;
		uint32_t memory_response_write = execute_COPY(instruction->param0, instruction->param1);
		LOG_WARNING("[CPU] :=> Executed COPY <%d> ([%d] =>[%d])", memory_response_write, instruction->param1, instruction->param0);
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

	void *send_stream = malloc(sizeof(operands_t));

	// Serializo
	memcpy(send_stream, &operands, sizeof(operands_t));

	// envio a memoria para leer, el operands que contiene la direc fisica y el id del pcb
	conexion_enviar_stream(g_cpu.conexion, RD, send_stream, sizeof(operands_t));

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
	//operands (direc. fisica y valor) y uint32_t (pcb->id)
	void *big_stream = malloc(sizeof(uint32_t) + sizeof(operands_t));

	memcpy(big_stream,&g_cpu.pcb->id, sizeof(uint32_t));

	void *send_stream = operandos_to_stream(operands);

	memcpy(big_stream+sizeof(uint32_t),send_stream,sizeof(operands_t));

	conexion_enviar_stream(g_cpu.conexion, WT, big_stream, sizeof(operands_t));

	free(send_stream);
	free(operands);
	free(big_stream);
}

uint32_t
execute_COPY(uint32_t param1, uint32_t param2)
{
	uint32_t read_value = execute_READ(param2);
	execute_WRITE(param1, read_value);

	return read_value;
}


// ============================================================================================================
//			   							***** MMU - TLB *****
// ============================================================================================================

/*
Tam memoria: 4096Bytes = 4KB = 2¹²
Tamaño de pag = Tamaño de frame = 64Bytes -> 2⁶ --> 6 bits de offset

Direc Logica:  |001000|111111|-> offset
			   | Pag  |  Off |


Tabla de Paginas:

|N° Pag | Frame |
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

uint32_t req_physical_address(cpu_t* cpu, uint32_t logical_address){

	uint32_t frame;
	uint32_t numero_tabla_de_segundo_nivel;

	//TODO --> Revisar el tipo de pcb-> page table: es void* por lo que me genera un warning al compilar
	numero_tabla_de_segundo_nivel = obtener_tabla_segundo_nivel(cpu->pcb->page_table,obtener_entrada_primer_nivel(logical_address, cpu->page_size, cpu->page_amount_entries));
	frame = obtener_frame(numero_tabla_de_segundo_nivel, obtener_entrada_segundo_nivel(logical_address, cpu->page_size, cpu->page_amount_entries));

	// Después actualizamos la TLB
	update_TLB(page_number(logical_address),frame);

	return frame * (cpu->page_size) + obtener_offset(logical_address, cpu->page_size);
}


uint32_t obtener_numero_pagina(uint32_t direccion_logica, uint32_t tamanio_pagina){
	return direccion_logica/tamanio_pagina;
}

uint32_t obtener_offset(uint32_t direccion_logica, uint32_t tamanio_pagina){
	return direccion_logica - tamanio_pagina * obtener_numero_pagina(direccion_logica, tamanio_pagina);
}

uint32_t obtener_entrada_primer_nivel(uint32_t direccion_logica, uint32_t tamanio_pagina, uint32_t cant_en_por_pag){
	return obtener_numero_pagina(direccion_logica, tamanio_pagina) / cant_en_por_pag;
}

uint32_t obtener_entrada_segundo_nivel(uint32_t direccion_logica, uint32_t tamanio_pagina, uint32_t cant_en_por_pag){
	return obtener_numero_pagina(direccion_logica, tamanio_pagina) % cant_en_por_pag;
}


uint32_t obtener_tabla_segundo_nivel(uint32_t tabla_primer_nivel, uint32_t desplazamiento){

	// ENVIO DE STREAM

	LOG_TRACE("[MMU] :=> Request Page of Second Table...");

	operands_t *operands = malloc(sizeof(operands_t));

	operands->op1 = tabla_primer_nivel;
	operands->op2 = desplazamiento;

	void *send_stream = operandos_to_stream(operands);

	conexion_enviar_stream(g_cpu.conexion, SND_PAGE, send_stream, sizeof(operands_t));

	// RECIBO DE UINT32_T

	uint32_t *ret_page_snd_level = connection_receive_value(g_cpu.conexion, sizeof(uint32_t));

	if (ret_page_snd_level == NULL){
		LOG_ERROR("[Memory-Client] :=> page_second_level can't be NULL");
		return VALOR_INVALIDO;
	}else{
		LOG_DEBUG("[MMU] :=> page_second_level is: %d", *ret_page_snd_level);
	}

	uint32_t ret_page = *ret_page_snd_level;

	free(ret_page_snd_level);
	free(send_stream);
	free(operands);

	return ret_page;

}


uint32_t obtener_frame(uint32_t tabla_segundo_nivel,uint32_t desplazamiento){

	// ENVIO DE STREAM

	LOG_TRACE("[MMU] :=> Request Frame value...");

	operands_t *operands = malloc(sizeof(operands_t));

	operands->op1 = tabla_segundo_nivel;
	operands->op2 = desplazamiento;

	void *send_stream = operandos_to_stream(operands);

	conexion_enviar_stream(g_cpu.conexion, FRAME, send_stream, sizeof(operands_t));

	// RECIBO DE UINT32_T

	uint32_t *frame = connection_receive_value(g_cpu.conexion, sizeof(uint32_t));

	if (frame == NULL){
		LOG_ERROR("[Memory-Client] :=> Frame can't be NULL");
		return VALOR_INVALIDO;
	}else{
		LOG_DEBUG("[MMU] :=> Frame is: %d", *frame);
	}

	uint32_t ret_frame = *frame;

	free(frame);
	free(send_stream);
	free(operands);

	return ret_frame;
}
