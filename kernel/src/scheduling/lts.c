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

void *
long_time_schedule(void *kernel_ref)
{
	kernel_t *kernel = kernel_ref;

	for (;;)
	{
		// Wait for a process to be created.
		WAIT(kernel->sync.dispatch);
		// TODO: Fetch from pcbs
	}

	return NULL;
}

void admit_process(kernel_t *kernel, pcb_t *pcb)
{
	pcb_unit_t *process = new_pcb_unit(pcb);
	// TODO: add to READY QUEUE
}
