/**
 * @file sts.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-03-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sts.h"
#include "kernel.h"
#include "pcb_unit.h"
#include "cpu_controller.h"
#include "log.h"

void execute(kernel_t *kernel, pcb_t *pcb);
void terminate(kernel_t *kernel, pcb_t *pcb);

void *short_term_schedule(void *data)
{
	LOG_TRACE("[STS] :=> Short Term Scheduling Running...");

	kernel_t *kernel = (kernel_t *)data;
	scheduler_t sched = kernel->scheduler;

	for (;;)
	{
		pcb_t *pcb = sched.get_next(&sched);

		if (pcb)
			execute(kernel, pcb);
		else
		{
			sleep(10);
		}
	}
}

void execute(kernel_t *kernel, pcb_t *pcb)
{
	pcb->status = PCB_EXECUTING;
	ssize_t bytes_sent = -1;

	bytes_sent = cpu_controller_send_pcb(kernel->conexion_dispatch, PCB, pcb);

	if (bytes_sent > 0)
	{
		LOG_INFO("[STS] :=> Executing PCB<%d>", pcb->id);
	}
	else
	{
		LOG_ERROR("[STS] :=> PCB could not be sent");
	}

	LOG_TRACE("[STS] :=> Awaiting for pcb to return...");
	pcb_destroy(pcb);
	pcb = NULL;
	uint32_t io_time = 0;
	pcb = (pcb_t *)cpu_controller_receive_pcb(kernel->conexion_dispatch, &io_time);
	LOG_INFO("[STS] :=> PCB #%d has returned", pcb->id);

	if (io_time > 0)
		LOG_WARNING("[STS] :=> IO (%d)", io_time);

	switch (pcb->status)
	{
	case PCB_BLOCKED:
		block(&kernel->scheduler, pcb, io_time);
		LOG_WARNING("[STS] :=> PCB<%d> is blocked", pcb->id);
		break;

	case PCB_TERMINATED:
		terminate(kernel, pcb);
		LOG_DEBUG("[STS] :=> PCB<%d> has exited", pcb->id);
		break;

	case PCB_READY:
		safe_queue_push(kernel->scheduler.ready, pcb);
		LOG_TRACE("[STS] :=> PCB<%d> is ready", pcb->id);
		break;

	default:
		terminate(kernel, pcb);
		LOG_ERROR("[STS] :=> PCB<%d> has a corrupted status (%d). Terminated.", pcb->id, pcb->status);
		break;
	}
}

void terminate(kernel_t *kernel, pcb_t *pcb)
{
	SIGNAL(kernel->scheduler.dom);
	pcb_destroy(pcb);
	pcb = NULL;
}

void block(scheduler_t *scheduler, pcb_t *pcb, uint32_t io_time)
{
	pcb->io = io_time;
	safe_queue_push(scheduler->blocked, pcb);
	SIGNAL(scheduler->io_request);
}
