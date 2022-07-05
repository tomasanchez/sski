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
#include "cpu_controller.h"

// ============================================================================================================
//                                   ***** Declarations *****
// ============================================================================================================

void admit(kernel_t *kernel);

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
		admit(kernel);
	}

	return NULL;
}

void admit(kernel_t *kernel)
{

	safe_queue_t *new = kernel->scheduler.new;
	safe_queue_t *ready = kernel->scheduler.ready;
	conexion_t memory = kernel->conexion_memory;

	if (new != NULL && ready != NULL)
	{
		LOG_TRACE("[LTS] :=> Admitting a new process...");
		// Sets to ready a new process and enqueues.
		pcb_t *pcb = safe_queue_pop(new);

		if (pcb != NULL)
		{
			pcb->status = PCB_NEW;
			LOG_INFO("[LTS] :=> Process <%d> changed status to NEW", pcb->id);

			if (should_interrupt(&kernel->scheduler, pcb))
			{

				ssize_t bytes_sent = cpu_controller_send_interrupt(kernel->conexion_interrupt);

				if (bytes_sent > 0)
				{
					LOG_TRACE("[LTS] :=> Interruption occurred [%ld bytes]", bytes_sent);
				}
				else
				{
					LOG_ERROR("[LTS] :=> Couldn't sent interruption");
				}
			}

			safe_queue_push(ready, pcb);

			// Request page table.
			if (conexion_esta_conectada(memory))
			{
				LOG_TRACE("[LTS] :=> Request page table...");
				accion_t *req_page = accion_create(NEW_PROCESS, 0);
				accion_enviar(req_page, memory.socket);
				accion_t *recv_page = accion_recibir(memory.socket);

				uint32_t *page_ref = malloc(sizeof(uint32_t));
				*page_ref = recv_page->param;
				pcb->page_table = page_ref;

				accion_destroy(req_page);
				accion_destroy(recv_page);
				LOG_DEBUG("[LTS] :=> Page table received");
			}
			else
			{
				LOG_WARNING("[LTS] :=> Memory is not connected");
			}

			LOG_TRACE("[LTS] :=> Process <%d> moved to Ready Queue", pcb->id);
		}
		else
		{
			LOG_ERROR("[LTS] :=> No process to be admit - PCB cannot be NULL");
		}
	}
	else
	{
		LOG_ERROR("[LTS] :=> Error while admitting a new process: NULL Queues");
	}
}
