/**
 * @file lts.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 05-10-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdlib.h>
#include "kernel.h"
#include "pcb_unit.h"
#include "safe_queue.h"
#include "log.h"
#include "accion.h"

// ============================================================================================================
//                                   ***** Declarations *****
// ============================================================================================================

/**
 * @brief Admits a new process
 *
 * @param new
 * @param ready
 * @param memory
 */
void admit(safe_queue_t *new, safe_queue_t *ready, conexion_t memory);

// ============================================================================================================
//                                   ***** Definitions *****
// ============================================================================================================

void *
long_term_schedule(void *kernel_ref)
{
	LOG_TRACE("[LTS] :=> Initializing Long Term Scheduler");

	kernel_t *kernel = kernel_ref;
	scheduler_t sched = kernel->scheduler;
	int dom = -1, request = -99;

	for (;;)
	{
		LOG_TRACE("[LTS] :=> Verifying Multiprogramming grade...");
		sem_getvalue(sched.dom, &dom);
		// Wait for programs to end...
		WAIT(sched.dom);
		LOG_DEBUG("[LTS] :=> There are <%d> available slots for processing", dom - 1);
		// Wait for a process to be created.
		sem_getvalue(sched.req_admit, &request);
		LOG_WARNING("[LTS] :=> There are <%d> previous requests", request);
		WAIT(sched.req_admit);
		admit(sched.new, sched.ready, kernel->conexion_memory);
		LOG_DEBUG("[LTS] :=> Process admitted");
	}

	return NULL;
}

void admit(safe_queue_t *new, safe_queue_t *ready, conexion_t memory)
{
	if (new != NULL && ready != NULL)
	{
		// Sets to ready a new process and enqueues.
		pcb_t *pcb = safe_queue_pop(new);

		if (pcb != NULL)
		{
			pcb->status = PCB_NEW;
			safe_queue_push(ready, pcb);

			// Request page table.
			if (conexion_esta_conectada(memory))
			{
				accion_t *req_page = accion_create(NEW_PROCESS, 0);
				accion_enviar(req_page, memory.socket);
				accion_t *recv_page = accion_recibir(memory.socket);

				uint32_t *page_ref = malloc(sizeof(uint32_t));
				*page_ref = recv_page->param;
				pcb->page_table = page_ref;

				accion_destroy(req_page);
				accion_destroy(recv_page);
			}
		}
	}
}
