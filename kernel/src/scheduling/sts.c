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
#include "time2.h"
#include "sts.h"
#include "kernel.h"
#include "pcb_unit.h"
#include "cpu_controller.h"
#include "log.h"
#include "cfg.h"
#include "mts.h"
#include "swap_controller.h"

extern kernel_t g_kernel;

void execute(kernel_t *kernel, pcb_t *pcb);
void terminate(kernel_t *kernel, pcb_t *pcb);
void pre_empt(scheduler_t *scheduler, pcb_t *pcb);
void re_schedule(pcb_t *pcb);

void *short_term_schedule(void *data)
{
	LOG_TRACE("[STS] :=> Short Term Scheduling Running...");

	kernel_t *kernel = NULL;
	kernel = (kernel_t *)data;
	scheduler_t sched = kernel->scheduler;

	for (;;)
	{
		WAIT(sched.execute);
		pcb_t *pcb = NULL;
		pcb = sched.get_next(&sched);

		if (pcb)
			execute(kernel, pcb);
	}

	return NULL;
}

void execute(kernel_t *kernel, pcb_t *pcb)
{
	// Update Status to Executing
	pcb->status = PCB_EXECUTING;
	// Time the CPU Usage
	struct timeval start, stop;
	uint32_t real_usage = 0;
	gettimeofday(&start, NULL);
	kernel->scheduler.current_estimation = pcb->estimation;

	// Send PCB to CPU so it can execute it
	ssize_t bytes_sent = -1;
	bytes_sent = cpu_controller_send_pcb(kernel->conexion_dispatch, PCB, pcb);

	if (bytes_sent > 0)
	{
		LOG_INFO("[STS] :=> Executing PCB #%d", pcb->id);
		LOG_WARNING("[STS] :=> PCB #%d estimated to use CPU for %dms", pcb->id, pcb->estimation);
		pcb_destroy(pcb);
		pcb = NULL;
	}
	else
	{
		// Fail fast - Terminate immediately
		LOG_ERROR("[STS] :=> PCB could not be sent - Terminated");
		terminate(kernel, pcb);
		return;
	}

	// WAIT for PCB to leave CPU - Include IO time
	uint32_t io_time = 0;
	pcb = (pcb_t *)cpu_controller_receive_pcb(kernel->conexion_dispatch, &io_time);

	// Time real CPU usage
	gettimeofday(&stop, NULL);
	real_usage = time_diff_ms(start, stop);
	pcb->real = real_usage;
	LOG_TRACE("[STS] :=> PCB #%d returned from CPU %dms", pcb->id, pcb->real);
	kernel->scheduler.current_estimation = 0;

	if (io_time > 0)
	{
		LOG_WARNING("[STS] :=> IO (%d)", io_time);
	}

	// According to PCB Status take different options
	switch (pcb->status)
	{
		// This means PCB went for I/O burst
	case PCB_BLOCKED:
		block(&kernel->scheduler, pcb, io_time);
		LOG_WARNING("[STS] :=> PCB #%d is blocked", pcb->id);
		break;

		// PCB has executed operation EXIT
	case PCB_TERMINATED:
		LOG_ERROR("[STS] :=> PCB #%d terminated", pcb->id);
		terminate(kernel, pcb);
		break;

		// PCB was preempted.
	case PCB_READY:
		pre_empt(&kernel->scheduler, pcb);
		LOG_TRACE("[STS] :=> PCB #%d was preempted, Remaining %dms", pcb->id, pcb->estimation);
		break;

		// Invalid status - terminate program immediately.
	default:
		LOG_ERROR("[STS] :=> PCB #%d has a corrupted status (%d). Terminated.", pcb->id, pcb->status);
		terminate(kernel, pcb);
		break;
	}

	SIGNAL(kernel->scheduler.execute);
}

void terminate(kernel_t *kernel, pcb_t *pcb)
{
	SIGNAL(kernel->scheduler.dom);
	swap_controller_exit(pcb);
	pcb_destroy(pcb);
	pcb = NULL;
}

void block(scheduler_t *scheduler, pcb_t *pcb, uint32_t io_time)
{
	pcb->io = io_time;
	re_schedule(pcb);
	safe_queue_push(scheduler->blocked, pcb);
	notify_mts(scheduler, pcb);
	SIGNAL(scheduler->io_request);
}

void event(scheduler_t *scheduler, pcb_t *pcb)
{
	LOG_TRACE("[STS] :=> PCB #%d has been unblocked", pcb->id);
	pcb->status = PCB_READY;
	safe_queue_push(scheduler->ready, pcb);
	check_interruption(&g_kernel, pcb);
	SIGNAL(scheduler->execute);
}

void pre_empt(scheduler_t *scheduler, pcb_t *pcb)
{
	if (pcb->real < pcb->estimation)
		pcb->estimation -= pcb->real;
	else
		pcb->estimation = pcb->real - pcb->estimation;

	safe_queue_push(scheduler->ready, pcb);
};

void re_schedule(pcb_t *pcb)
{
	double alpha = alfa();

	pcb->estimation = (uint32_t)(pcb->real * alpha) + (uint32_t)((1 - alpha) * pcb->estimation);
}
