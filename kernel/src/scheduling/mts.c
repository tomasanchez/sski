/**
 * @file mts.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-06-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "mts.h"
#include "pcb.h"
#include "time.h"
#include "log.h"

void *track_time(void *scheduler_data);

void notify_mts(scheduler_t *scheduler, pcb_t *pcb)
{
	LOG_TRACE("[MTS] :=> Notified of PCB #%d...", pcb->id);

	mts_dto_t *dto = malloc(sizeof(mts_dto_t));

	dto->scheduler = scheduler;
	dto->pcb = pcb;

	thread_manager_launch(&scheduler->tm, track_time, dto);
}

void *track_time(void *dto)
{
	LOG_WARNING("[MTS] :=> Tracking...");

	// Cast to type SCHEDULER
	scheduler_t *s = ((mts_dto_t *)dto)->scheduler;
	// Cast to type PCB
	pcb_t *pcb = ((mts_dto_t *)dto)->pcb;

	if (pcb == NULL || s == NULL)
	{
		LOG_ERROR("[MTS] :=> PCB or Scheduler NULL - THIS SHOULD NEVER HAPPEN");
		return NULL;
	}

	// Get current time
	LOG_DEBUG("[MTS] :=> Tracking PCB #%d", pcb->id);
	sleep(s->max_blocked_time / 1000);
	LOG_TRACE("[MTS] :=> Suspension Tracker for PCB #%d finished", pcb->id);

	if (pcb_exists(s->blocked, pcb->id))
	{
		LOG_INFO("[MTS] :=> PCB has been blocked for %dms", s->max_blocked_time);
		suspend(s, pcb);
	}
	else
	{
		LOG_WARNING("[MTS] :=> Nothing to be done for PCB #%d", pcb->id);
	}

	free(dto);
	thread_manager_end_thread(&s->tm);

	return NULL;
}

void suspend(scheduler_t *scheduler, pcb_t *pcb)
{
	pcb->status = PCB_SUSPENDED_BLOCKED;
	pcb_remove_by_id(scheduler->blocked, pcb->id);
	safe_queue_push(scheduler->blocked_sus, pcb);
	LOG_WARNING("[MTS] :=> Blocked PCB #%d has been SUSPENDED", pcb->id);
	SIGNAL(scheduler->dom);
}

pcb_t *resume(scheduler_t *scheduler)
{
	pcb_t *pcb = NULL;

	pcb = safe_queue_pop(scheduler->ready_sus);

	if (pcb != NULL)
	{
		LOG_WARNING("[MTS] :=> Suspended PCB #%d has been RESUMED", pcb->id);
	}

	return pcb;
}

void suspended_event(scheduler_t *scheduler, pcb_t *pcb)
{
	LOG_DEBUG("[MTS] :=> Event occured for suspended PCB #%d", pcb->id);
	pcb->status = PCB_SUSPENDED_BLOCKED;
	safe_queue_push(scheduler->ready_sus, pcb);
	LOG_TRACE("[MTS] :=> PCB #%d moved to SUSPENDED Ready", pcb->id);
	SIGNAL(scheduler->req_admit);
}
