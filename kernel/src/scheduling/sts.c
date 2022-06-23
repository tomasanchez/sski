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

void *short_term_schedule(void *data)
{
	kernel_t *kernel = (kernel_t *)data;
	scheduler_t sched = kernel->scheduler;

	for (;;)
	{
		pcb_t *pcb = sched.get_next(&sched);
		execute(kernel, pcb);
	}
}

void execute(kernel_t *kernel, pcb_t *pcb)
{
	pcb->status = PCB_EXECUTING;
	ssize_t bytes_sent = -1;

	bytes_sent = cpu_controller_send_pcb(kernel->conexion_dispatch, PCB, pcb);

	if (bytes_sent > 0)
	{
		LOG_INFO("Executing PCB(id=%d)", pcb->id);
	}
	else
	{
		LOG_ERROR("PCB could not be sent");
	}

	pcb_destroy(pcb);
	pcb = NULL;
	pcb = (pcb_t *)cpu_controller_receive_pcb(kernel->conexion_dispatch);

	switch (pcb->status)
	{
	case PCB_BLOCKED:
		break;

	default:
		safe_queue_push(kernel->scheduler.ready, pcb);
		break;
	}
}

pcb_t *recover_pcb(kernel_t *kernel)
{
	return NULL;
}
