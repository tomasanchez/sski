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
#include "cpu.h"

extern cpu_t g_cpu;

// ============================================================================================================
//                                   ***** Definitions  *****
// ============================================================================================================

#define LOG_PCB(pcb)                                                                                                       \
	{                                                                                                                      \
		LOG_INFO("[Server] :=> PCB<%d>(size: %lu, estimation: %d, pc: %d)", pcb->id, pcb->size, pcb->estimation, pcb->pc); \
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
	g_cpu.pcb = pcb;
	return pcb;
}

ssize_t return_pcb(int fd, pcb_t *pcb, uint32_t time)
{
	if (fd < 0)
	{
		LOG_ERROR("[Server] :=> Invalid file descriptor");
		return -1;
	}

	if (pcb == NULL)
	{
		LOG_ERROR("[Server] :=> PCB to be returned is NULL");
		return -1;
	}

	LOG_WARNING("[Server] :=> Returning PCB...");
	LOG_PCB(pcb);

	size_t pcb_size = pcb_bytes_size(pcb);
	void *stream = malloc(pcb_size + sizeof(time));
	void *pcb_stream = pcb_to_stream(pcb);

	memcpy(stream, pcb_stream, pcb_size);
	memcpy(stream + pcb_size, &time, sizeof(time));

	ssize_t bytes_sent = servidor_enviar_stream(INOUT, fd, stream, pcb_size + sizeof(time));

	if (bytes_sent > 0)
	{
		LOG_DEBUG("[Server] :=> PCB<%d> returned to Kernel [%ld bytes]", pcb->id, bytes_sent);
	}
	else
	{
		LOG_ERROR("[Server] :=> PCB<%d> did not return", pcb->id);
	}

	pcb_destroy(pcb);
	g_cpu.pcb = NULL;

	return bytes_sent;
}
