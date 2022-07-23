/**
 * @file kernel_controller.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-19-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "fs.h"
#include "kernel_controller.h"
#include "server.h"
#include "pcb.h"
#include "cfg.h"
#include "log.h"
#include "os_memory.h"
#include "swap.h"

// ============================================================================================================
//                                   ***** Declarations *****
// ============================================================================================================

extern memory_t g_memory;

/**
 * @brief Swaps a PCB into the memory
 *
 * @param pcb_stream to swap
 * @return memory position
 */
uint32_t
swap_pcb_(void *pcb_stream);

pcb_t *
retrieve_swapped_pcb(uint32_t pcb_id);

/**
 * @brief Deletes swapped file
 *
 * @param pcb_id to identfy the swapped_partition
 */
void delete_swapped_pcb(uint32_t pcb_id);

/**
 * @brief Get a free page table index
 *
 * @return the page table index
 */
uint32_t
get_page_table(void);

// ============================================================================================================
//                                   ***** Public Functions *****
// ============================================================================================================

void kernel_controller_swap(int socket)
{
	ssize_t bytes_received = -1;

	void *pcb_stream = servidor_recibir_stream(socket, &bytes_received);
	LOG_TRACE("[Server] :=> A PCB was received to be swapped");

	uint32_t swap_status = swap_pcb_(pcb_stream);
	if (swap_status == SUCCESS)
	{
		LOG_TRACE("[Server] :=> PCB was SUCCESSSFULLY swapped");
	}
	else
	{
		LOG_TRACE("[Server] :=> Failed to swap PCB");
	}

	free(pcb_stream);
}

void kernel_controller_read_swap(int socket)
{
	ssize_t bytes_received = -1;

	uint32_t *pcb_id = (uint32_t *)servidor_recibir_stream(socket, &bytes_received);
	LOG_TRACE("[Server] :=> A PCB ID #%d was received", *pcb_id);

	unswap_pcb(*pcb_id);

	free(pcb_id);
}

void kernel_controller_destroy_process_file(int socket)
{
	ssize_t bytes_received = -1;

	void *stream = (uint32_t *)servidor_recibir_stream(socket, &bytes_received);
	uint32_t pcb_id, table_id;
	memcpy(&pcb_id, stream, sizeof(uint32_t));
	memcpy(&table_id, stream + sizeof(uint32_t), sizeof(uint32_t));
	LOG_TRACE("[Server] :=> PCB #%d requested termination. Deleting Table #%d", pcb_id, table_id);
	delete_process(&g_memory, table_id);
	delete_swapped_pcb(pcb_id);
	free(stream);
	LOG_INFO("[Server] :=> PCB #%d deleted", pcb_id);

	LOG_DEBUG("[Server] :=> \tCurrent\tTables(%d)", safe_list_size(g_memory.tables_lvl_1));
	for (uint32_t i = 0; i < (uint32_t)safe_list_size(g_memory.tables_lvl_1); i++)
	{
		page_table_lvl_1_t *table = safe_list_get(g_memory.tables_lvl_1, i);
		LOG_WARNING("\tTable\t#%d", i);
		if (table == NULL)
		{
			LOG_ERROR("Table #%d was recently deleted", i);
		}
		else
		{
			print_table(&g_memory, i);
		}
	}
}

void kernel_controller_memory_init(int socket)
{
	ssize_t bytes_received = -1;
	void *ref = servidor_recibir_stream(socket, &bytes_received);

	if (bytes_received <= 0)
	{
		LOG_ERROR("[Server] :=> Could not receive PID");
		return;
	}

	operands_t operands = operandos_from_stream(ref);
	uint32_t pid = operands.op1;
	uint32_t pcb_size = operands.op2;
	swap_data_t *swap_data = new_swap_data(pid, pcb_size);
	safe_list_add(g_memory.swap_data, swap_data);
	free(ref);

	LOG_DEBUG("[Server] :=> Initializing PCB#%d [%dbytes]", pid, pcb_size);
	LOG_TRACE("[Memory] :=> Creating SWAP file for PCB #%d...", pid);
	create_file(pid, pcb_size);
	LOG_TRACE("[Server] :=> Obtaining available page table");

	uint32_t page_table = get_page_table();

	LOG_INFO("[Server] :=> Page Table <%d> was obtained", page_table);

	ssize_t bytes_sent = servidor_enviar_stream(MEMORY_INIT, socket, &page_table, sizeof(page_table));

	if (bytes_sent <= 0)
	{
		LOG_ERROR("[Server] :=> Page table could not be sent");
	}
	else
	{
		LOG_DEBUG("[Server] :=> Page table <%d> was sent [%ld bytes]", page_table, bytes_sent);
		LOG_DEBUG("[Server] :=> \tCurrent\tTables(%d)", safe_list_size(g_memory.tables_lvl_1));
		for (uint32_t i = 0; i < (uint32_t)safe_list_size(g_memory.tables_lvl_1); i++)
		{
			page_table_lvl_1_t *table = safe_list_get(g_memory.tables_lvl_1, i);
			LOG_WARNING("\tTable\t#%d", i);
			if (table == NULL)
			{
				LOG_ERROR("Table #%d was recently deleted", i);
			}
			else
			{
				print_table(&g_memory, i);
			}
		}
	}
}

// ============================================================================================================
//                                   ***** Private Functions *****
// ============================================================================================================

void delete_swapped_pcb(uint32_t pid)
{
	delete_swap_file(pid);
	delete_swap_data(&g_memory, pid);
}

uint32_t
get_page_table(void)
{
	return create_new_process(&g_memory);
}
