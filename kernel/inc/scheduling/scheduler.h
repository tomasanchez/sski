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

typedef struct Scheduler
{
	// Degree Of Multiprogramming
	sem_t dom;
	// Request to admit
	sem_t req_admit;
	// NEW Queue
	safe_queue_t *new;
	// READY Queue
	safe_queue_t *ready;
	// Get scheduler next
	void *(*get_next)(scheduler_t);
} scheduler_t;

/**
 * @brief Instantiates a new scheduler
 *
 * @param dom the Degree Of Multiprogramming
 * @return a prepared struct
 */
scheduler_t new_scheduler(int dom);

/**
 * @brief Destroys the scheduler data.
 *
 */
void scheduler_delete(scheduler_t scheduler);
