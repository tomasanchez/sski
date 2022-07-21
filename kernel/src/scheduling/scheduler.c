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
#include "io_scheduler.h"
#include "pcb_unit.h"
#include "time.h"
#include <string.h>
#include "kernel.h"
#include "cpu_controller.h"
#include "scheduler_algorithms.h"

scheduler_t new_scheduler(int dom, char *algorithm, uint32_t max_blocked_time)
{
	scheduler_t s;

	// Init queues
	s.new = new_safe_queue();
	s.ready = new_safe_queue();
	s.ready_sus = new_safe_queue();
	s.blocked = new_safe_queue();
	s.blocked_sus = new_safe_queue();

	// Init Algorithm
	s.get_next = strcmp(algorithm, "FIFO") == 0 ? get_next_fifo : get_next_srt;

	// Time Stamps
	s.current_estimation = 0;
	s.max_blocked_time = max_blocked_time;

	// Init Thread Manager
	s.tm = new_thread_manager();

	// Init semaphores
	s.dom = malloc(sizeof(sem_t));
	s.io_request = malloc(sizeof(sem_t));
	s.req_admit = malloc(sizeof(sem_t));
	s.execute = malloc(sizeof(sem_t));
	sem_init(s.dom, SHARE_BETWEEN_THREADS, dom);
	sem_init(s.io_request, SHARE_BETWEEN_THREADS, 0);
	sem_init(s.req_admit, SHARE_BETWEEN_THREADS, 0);
	sem_init(s.execute, SHARE_BETWEEN_THREADS, 0);

	return s;
}

void scheduler_start(scheduler_t *scheduler)
{
	thread_manager_launch(&scheduler->tm, io_scheduler, scheduler);
}

void scheduler_delete(scheduler_t scheduler)
{

	// Destroy queues
	safe_queue_destroy(scheduler.new, pcb_destroy);
	safe_queue_destroy(scheduler.ready, pcb_destroy);
	safe_queue_destroy(scheduler.ready_sus, pcb_destroy);
	safe_queue_destroy(scheduler.blocked, pcb_destroy);
	safe_queue_destroy(scheduler.blocked_sus, pcb_destroy);

	// Destroy Thread Manager
	thread_manager_destroy(&scheduler.tm);

	// Destroy semaphores
	sem_destroy(scheduler.dom);
	free(scheduler.dom);
	sem_destroy(scheduler.req_admit);
	free(scheduler.req_admit);
	sem_destroy(scheduler.io_request);
	free(scheduler.io_request);
	sem_destroy(scheduler.execute);
	free(scheduler.execute);
}

void *schedule(void *data)
{
	scheduler_t *scheduler = (scheduler_t *)data;
	return scheduler;
}

bool should_interrupt(scheduler_t *scheduler, pcb_t *pcb)
{
	return scheduler->current_estimation > pcb->estimation;
}

void check_interruption(void *kernel_ref, pcb_t *pcb)
{
	kernel_t *kernel = (kernel_t *)kernel_ref;

	if (should_interrupt(&kernel->scheduler, pcb))
	{
		ssize_t bytes_sent = cpu_controller_send_interrupt(kernel->conexion_interrupt);

		if (bytes_sent > 0)
		{
			LOG_ERROR("[LTS] :=> Interruption occurred for PCB#%d [%ld bytes]", pcb->id, bytes_sent);
			LOG_INFO("[LTS] :=> Estimations{Current: %dms, New: %dms}", kernel->scheduler.current_estimation, pcb->estimation);
		}
		else
		{
			LOG_ERROR("[LTS] :=> Couldn't sent interruption");
		}
	}
	else
	{
		LOG_ERROR("[LTS] :=> No interruption required");
		LOG_INFO("[LTS] :=> Estimations{Current: %dms, New: %dms}", kernel->scheduler.current_estimation, pcb->estimation);
	}
}
