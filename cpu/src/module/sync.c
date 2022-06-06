/**
 * @file sync.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-05-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "sync.h"

cpu_sync_t
init_sync()
{
	cpu_sync_t sync;

	sem_init(&sync.pcb_received, SHARE_BETWEEN_THREADS, 0);

	return sync;
}

void sync_destroy(cpu_sync_t *sync)
{
	sem_destroy(&sync->pcb_received);
}
