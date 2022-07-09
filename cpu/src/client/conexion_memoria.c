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
#include "tlb.h"

// ============================================================================================================
//                                   ***** Definiciones y Estructuras  *****
// ============================================================================================================

extern cpu_t g_cpu;
extern tlb_t *TLB;

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

	opcode_t req_pg_size = SZ;

	ssize_t bytes_sent = -1;
	bytes_sent = connection_send_value(cpu->conexion, &req_pg_size, sizeof(req_pg_size));

	if (bytes_sent <= 0)
	{
		LOG_ERROR("[Memory-Client] :=> Nothing was sent - THIS SHOULD NEVER HAPPEN");
		return;
	}
	else
	{
		LOG_WARNING("[Memory-Client] :=> Requested Page Size [%ld bytes]", bytes_sent);
	}

	uint32_t *pg_size = connection_receive_value(cpu->conexion, sizeof(uint32_t));

	if (pg_size == NULL)
	{
		LOG_ERROR("[Memory-Client] :=> Page Size can't be NULL");
		return;
	}
	else
	{
		LOG_DEBUG("[MMU] :=> Page Size is: %d", *pg_size);
		cpu->page_size = *pg_size;
	}

	free(pg_size);
	LOG_WARNING("[MMU] :=> Page Size after free: %d", cpu->page_size);

	LOG_TRACE("[MMU] :=> Request Entries per Pages...");
	opcode_t req_am_entry = ENTRIES;

	ssize_t bytes_sent_for_am_entry = connection_send_value(cpu->conexion, &req_am_entry, sizeof(req_am_entry));

	if (bytes_sent_for_am_entry <= 0)
	{
		LOG_ERROR("[Memory-Client] :=> Nothing was sent - THIS SHOULD NEVER HAPPEN");
		return;
	}
	else
	{
		LOG_DEBUG("[Memory-Client] :=> Requested Amount Entry [%ld bytes]", bytes_sent_for_am_entry);
	}

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

	tlb_init();

	handshake(cpu);

	return NULL;
}
