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

// ============================================================================================================
//                                   ***** Public Functions *****
// ============================================================================================================

void kernel_controller_swap(int socket)
{
	ssize_t bytes_received = -1;
	void *pcb_stream = servidor_recibir_stream(socket, &bytes_received);

	LOG_TRACE("A PCB was received to be swapped");
	uint32_t memory_position = swap_pcb(pcb_stream);
	LOG_TRACE("PCB  was swapped to position <%d>", memory_position);

	ssize_t bytes_sent = servidor_enviar_stream(SWAP, socket, &memory_position, sizeof(memory_position));

	if (bytes_sent > 0)
	{
		LOG_DEBUG("Memory position <%d> was sent [%d bytes]", memory_position, bytes_sent);
	}
	else
	{
		LOG_ERROR("Memory position could not be sent.");
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
