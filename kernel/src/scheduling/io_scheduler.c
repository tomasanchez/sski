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
#include "io_scheduler.h"
#include "mts.h"
#include "sts.h"
#include "pcb.h"
#include "scheduler.h"
#include "kernel.h"

extern kernel_t *g_kernel;

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
		int requests = 0;
		sem_getvalue(s->io_request, &requests);

		if (requests > 0)
		{
			LOG_TRACE("[IO] :=> There are <%d> IO requests", requests);
		}
		else
		{
			LOG_WARNING("[IO] :=>  Waiting for an IO request");
		}

		WAIT(s->io_request);

		LOG_TRACE("[IO] :=> Attending a Request...");

		pcb_t *pcb = NULL;
		pcb = retrieve_pcb(s);

		if (pcb)
		{
			// IO request received
			LOG_INFO("[IO] :=> PCB #%d requested IO", pcb->id);
			s->current_io = pcb->id;
			// Execute IO Burst
			LOG_TRACE("[IO] :=> PCB #%d Using IO for: %ds", pcb->id, pcb->io);
			usleep(pcb->io * 1000);

			LOG_INFO("[IO] :=> PCB #%d IO finished", pcb->id);

			switch (pcb->status)
			{
			case PCB_SUSPENDED_BLOCKED:
				suspended_event(s, pcb);
				break;

			default:
				event(s, pcb);
				break;
			}
		}
		else
		{
			LOG_ERROR("[IO] :=> No PCB available when IO was signaled");
		}

		s->current_io = UINT32_MAX;
	}

	return NULL;
}

static pcb_t *
retrieve_pcb(scheduler_t *scheduler)
{
	pcb_t *pcb = NULL;

	pcb = safe_queue_pop(scheduler->blocked_sus);

	if (pcb == NULL)
	{
		pcb = safe_queue_pop(scheduler->blocked);

		if (pcb == NULL)
		{
			LOG_ERROR("[IO] :=> No PCB available - THIS SHOULD NEVER HAPPEN");
			return NULL;
		}

		LOG_DEBUG("[IO] :=> Handling IO request for PCB #%d, which was <BLOCKED>", pcb->id);
	}
	else
	{
		LOG_WARNING("[IO] :=> Handling IO request for PCB #%d, which was <SUSPENDED>", pcb->id);
	}

	return pcb;
}
