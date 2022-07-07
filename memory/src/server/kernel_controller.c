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

#include "kernel_controller.h"
#include "server.h"
#include "pcb.h"
#include "log.h"

// ============================================================================================================
//                                   ***** Declarations *****
// ============================================================================================================

/**
 * @brief Swaps a PCB into the memory
 *
 * @param pcb_stream to swap
 * @return memory position
 */
uint32_t
swap_pcb(void *pcb_stream);

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

	uint32_t memory_position = swap_pcb(pcb_stream);
	LOG_TRACE("[Server] :=> PCB  was swapped to position <%d>", memory_position);

	ssize_t bytes_sent = servidor_enviar_stream(SWAP, socket, &memory_position, sizeof(memory_position));

	if (bytes_sent > 0)
	{
		LOG_DEBUG(" [Server] :=> Memory position <%d> was sent [%ld bytes]", memory_position, bytes_sent);
	}
	else
	{
		LOG_ERROR("[Server] :=> Memory position could not be sent.");
	}
}

void kernel_controller_memory_init(int socket)
{
	ssize_t bytes_received = -1;
	void *pid_ref = servidor_recibir_stream(socket, &bytes_received);

	if (bytes_received <= 0)
	{
		LOG_ERROR("[Server] :=> Could not receive PID");
		return;
	}

	uint32_t pid = *(uint32_t *)pid_ref;
	free(pid_ref);

	LOG_DEBUG("[Server] :=> Initializing PCB #%d", pid);

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
	}
}

// ============================================================================================================
//                                   ***** Private Functions *****
// ============================================================================================================

uint32_t
swap_pcb(void *pcb_stream)
{
	free(pcb_stream);
	return (uint32_t)rand();
}

uint32_t
get_page_table(void)
{
	return (uint32_t)rand();
}
