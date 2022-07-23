/**
 * @file scheduler.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-02-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "sem.h"
#include "safe_queue.h"
#include "pcb.h"
#include "thread_manager.h"

typedef struct Scheduler
{
	// Degree Of Multiprogramming
	sem_t *dom;
	// Request to admit
	sem_t *req_admit;
	// Request to admit
	sem_t *io_request;
	// Allows Execution
	sem_t *execute;
	// Allows interruptions
	bool interrupt;
	// NEW Queue
	safe_queue_t *new;
	// READY Queue
	safe_queue_t *ready;
	// SUSPENDED READY Queue
	safe_queue_t *ready_sus;
	// BLOCKED Queue
	safe_queue_t *blocked;
	// SUSPENDED BLOCKED queue
	safe_queue_t *blocked_sus;

	// Current PCB estimation - Needed to preempt
	uint32_t current_estimation;

	// Max blocked time [ms]
	uint32_t max_blocked_time;

	// ID of the PCB executing IO
	uint32_t current_io;

	// Thread Tracker dependency.
	thread_manager_t tm;

	// Get scheduler next
	void *(*get_next)(void *);
} scheduler_t;

/**
 * @brief Instantiates a new scheduler
 *
 * @param dom the Degree Of Multiprogramming
 * @return a prepared struct
 */
scheduler_t new_scheduler(int dom, char *algorithm, uint32_t max_blocked_time);

/**
 * @brief Destroys the scheduler data.
 *
 */
void scheduler_delete(scheduler_t scheduler);

/**
 * @brief Schedules the IO manager
 *
 * @param scheduler
 */
void scheduler_start(scheduler_t *scheduler);

/**
 * @brief Verifies wether an interruption should be risen
 *
 * @param scheduler the scheduler object
 * @param pcb the process control block
 */
bool should_interrupt(scheduler_t *scheduler, pcb_t *pcb);

/**
 * @brief interrupts if necessary
 *
 * @param kernel the module instance
 * @param pcb the process control block
 */
void check_interruption(void *kernel, pcb_t *pcb);
