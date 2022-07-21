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
#include "opcode.h"
#include "swap_controller.h"

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

	uint32_t pid = pcb->id;

	// Get current time
	LOG_DEBUG("[MTS] :=> Tracking PCB #%d", pid);
	sleep(s->max_blocked_time / 1000);
	LOG_TRACE("[MTS] :=> Suspension Tracker for PCB #%d finished", pid);

	if (pcb_exists(s->blocked, pid))
	{
		LOG_INFO("[MTS] :=> PCB #%d has been blocked for %dms", pid, s->max_blocked_time);
		suspend(s, pcb);
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

	// meter lo del swap
	swap_controller_send_pcb(SWAP_PCB, pcb);

	LOG_WARNING("[MTS] :=> Blocked PCB #%d has been SUSPENDED", pcb->id);
	SIGNAL(scheduler->dom);
}

pcb_t *resume(scheduler_t *scheduler)
{
	pcb_t *pcb = NULL;

	pcb = safe_queue_pop(scheduler->ready_sus);

	if (pcb != NULL)
	{
		swap_controller_request_pcb(pcb->id);
		pcb_destroy(pcb);
		pcb = NULL;
		pcb = swap_controller_receive_pcb();

		LOG_WARNING("[MTS] :=> Suspended PCB #%d has been RESUMED", pcb->id);
	}

	return pcb;
}

void suspended_event(scheduler_t *scheduler, pcb_t *pcb)
{
	LOG_DEBUG("[MTS] :=> Event occured for suspended PCB #%d", pcb->id);
	pcb->status = PCB_SUSPENDED_READY;
	safe_queue_push(scheduler->ready_sus, pcb);
	LOG_TRACE("[MTS] :=> PCB #%d moved to SUSPENDED Ready", pcb->id);
	SIGNAL(scheduler->req_admit);
}
