/**
 * @file conexion_memoria.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar> with Francisco Maver <fmaver@fbra.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-07-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "conexion_memoria.h"
#include "cpu.h"
#include "conexion.h"
#include "log.h"
#include "cfg.h"
#include "module.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern cpu_t g_cpu;

// ============================================================================================================
//                               ***** Private Functions *****
// ============================================================================================================

static int conexion_init(cpu_t *cpu)
{
	char *port = puerto_memoria();
	char *ip = ip_memoria();

	LOG_DEBUG("[CPU:Client-Memory] :=> Connecting <Cpu> at %s:%s", ip, port);

	cpu->conexion = conexion_cliente_create(ip, port);

	if (on_module_connect(&cpu->conexion, false) EQ SUCCESS)
	{
		LOG_DEBUG("[CPU:Client-Memory] :=> Connected at %s:%s", ip, port);
	}

	return SUCCESS;
}

void handshake(cpu_t *cpu)
{

	LOG_TRACE("[MMU] :=> Request page table size...");

	opcode_t req_pg_size = DC;
	connection_send_value(cpu->conexion, &req_pg_size, sizeof(req_pg_size));
	uint32_t *pg_size = connection_receive_value(cpu->conexion, sizeof(uint32_t));
	
	cpu->page_size = *pg_size;

	free(pg_size);
	LOG_DEBUG("[MMU] :=> Page table size is: %d", cpu->page_size);

	LOG_TRACE("[MMU] :=> Request amount_entries_per_page...");
	opcode_t req_am_entry = DC;
	connection_send_value(cpu->conexion, &req_am_entry, sizeof(req_am_entry));
	uint32_t *am_entry = connection_receive_value(cpu->conexion, sizeof(uint32_t));
	
	cpu->page_amount_entries = *am_entry;

	free(am_entry);
	LOG_DEBUG("[MMU] :=> Amount Entries per page: %d", cpu->page_amount_entries);
}

// ============================================================================================================
//                               ***** Public Functions *****
// ============================================================================================================

void *routine_conexion_memoria(void *data)
{
	cpu_t *cpu = data;

	conexion_init(cpu);

	handshake(cpu);

	return NULL;
}
