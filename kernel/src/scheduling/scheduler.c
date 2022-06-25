/**
 * @file scheduler.c
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-02-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "scheduler.h"
#include "pcb_unit.h"
#include "scheduler_algorithms.h"

scheduler_t new_scheduler(int dom)
{
	scheduler_t s;

	// Init queues
	s.new = new_safe_queue();
	s.ready = new_safe_queue();
	s.get_next = get_next_fifo;

	s.dom = malloc(sizeof(sem_t));
	s.req_admit = malloc(sizeof(sem_t));

	// Init semaphores
	sem_init(s.dom, SHARE_BETWEEN_THREADS, dom);
	sem_init(s.req_admit, SHARE_BETWEEN_THREADS, 0);
	return s;
}

void scheduler_delete(scheduler_t scheduler)
{

	// Destroy queues
	safe_queue_destroy(scheduler.new, pcb_unit_destroy);
	safe_queue_destroy(scheduler.ready, pcb_unit_destroy);

	// Destroy semaphores
	sem_destroy(scheduler.dom);
	free(scheduler.dom);
	sem_destroy(scheduler.req_admit);
	free(scheduler.req_admit);
}

void *schedule(void *data)
{
	scheduler_t *scheduler = (scheduler_t *)data;
	return scheduler;
}
