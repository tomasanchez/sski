/**
 * @file io_scheduler.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-01-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "scheduler.h"
#include "pcb.h"

/**
 * @brief Retrieves a Blocked PCB
 *
 * @param scheduler the scheduler which handles the queues
 * @return a blcoked PCB
 */
static pcb_t *
retrieve_pcb(scheduler_t *scheduler);

void *io_scheduler(void *scheduler)
{

	// The Kernel's Scheduler unit
	scheduler_t *s = (scheduler_t *)scheduler;

	if (s == NULL)
	{
		LOG_ERROR("[IO] :=>  Scheduler is NULL.");
		return NULL;
	}
	LOG_DEBUG("[IO] :=>  IO Scheduler Unit ready.");

	for (;;)
	{
		LOG_WARNING("[IO] :=>  Waiting for an IO request");
		// WAIT for a process to need IO
		WAIT(s->io_request);
		LOG_TRACE("[IO] :=>  PCB requested IO");
		pcb_t *pcb = NULL;
		pcb = retrieve_pcb(s);

		if (pcb)
		{
			// IO request received
			LOG_DEBUG("[IO] :=> request received from Process #%d", pcb->id);

			// Execute IO Burst
			LOG_TRACE("[IO] :=> Using IO for: %ds", pcb->io);
			sleep(pcb->io / 1000);

			// Enqueue
			LOG_DEBUG("[IO] :=> IO finished");
			pcb->status = PCB_READY;
			safe_queue_push(s->ready, pcb);
		}
		else
		{
			LOG_ERROR("[IO] :=> No PCB available when IO was signaled");
		}
	}

	return NULL;
}

static pcb_t *
retrieve_pcb(scheduler_t *scheduler)
{
	if (scheduler->blocked == NULL)
	{
		LOG_ERROR("[IO] :=> No BLOCKED queue available");
		return NULL;
	}

	return safe_queue_pop(scheduler->blocked);
}
