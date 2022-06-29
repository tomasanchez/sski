/**
 * @file pcb_controller.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 04-23-2022
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdlib.h>
#include "pcb_controller.h"
#include "log.h"

// ============================================================================================================
//                                   ***** Definitions  *****
// ============================================================================================================

#define LOG_PCB(pcb)                                                                                                             \
	{                                                                                                                            \
		LOG_INFO("PCB= [id: %d, size: %lu, estimation: %d, program_counter: %d]", pcb->id, pcb->size, pcb->estimation, pcb->pc); \
	}

// ============================================================================================================
//                                   ***** Public Functions  *****
// ============================================================================================================

pcb_t *receive_pcb(int fd)
{
	ssize_t recv_bytes = -1;

	pcb_t *pcb = NULL;
	pcb = pcb_from_stream(servidor_recibir_stream(fd, &recv_bytes));
	LOG_PCB(pcb);
	return pcb;
}

ssize_t return_pcb(int fd, pcb_t *pcb, uint32_t time)
{
	size_t pcb_size = pcb_bytes_size(pcb);
	void *stream = malloc(pcb_size + sizeof(time));
	void *pcb_stream = pcb_to_stream(pcb);

	memcpy(stream, pcb_stream, pcb_size);
	memcpy(stream + pcb_size, &time, sizeof(time));

	return servidor_enviar_stream(INOUT, fd, stream, pcb_size + sizeof(time));
}
