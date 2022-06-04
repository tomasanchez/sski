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

void *short_term_schedule(void *data)
{
	kernel_t *kernel = (kernel_t *)data;
	scheduler_t sched = kernel->scheduler;

	for (;;)
	{
		pcb_unit_t *pcb = sched.get_next(sched);
		execute(kernel, pcb);
	}
}

void execute(kernel_t *kernel, pcb_unit_t *pcb)
{
	pcb->state = EXECUTING;

	void *stream = pcb_to_stream(pcb->_pcb);
	kernel->dispatch_dto.stream = stream;
	kernel->dispatch_dto.size = pcb_size(pcb);
	SIGNAL(kernel->sync.dispatch_req);
	WAIT(kernel->sync.dispatch_sent);
	free(stream);
}
