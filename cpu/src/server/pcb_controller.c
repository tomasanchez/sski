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
