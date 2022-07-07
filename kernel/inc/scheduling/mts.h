/**
 * @file mts.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-06-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "scheduler.h"

typedef struct MTS_DTO
{
	// A Scheduler reference
	scheduler_t *scheduler;
	// Spied PCB
	pcb_t *pcb;
} mts_dto_t;

/**
 * @brief Notifyes the MTS that a process has been blocked so it can track the PCB blocked time.
 *
 * @param scheduler the Kernel scheduler unit.
 * @param pcb the PCB that has been blocked.
 */
void notify_mts(scheduler_t *scheduler, pcb_t *pcb);

/**
 * @brief Suspends a PCB
 *
 * @param scheduler the Kernel scheduler unit.
 * @param pcb a blocked PCB
 */
void suspend(scheduler_t *scheduler, pcb_t *pcb);

/**
 * @brief Resumes the next PCB
 *
 * @param scheduler the Kernel scheduler unit
 * @return the next, suspended but ready, PCB in the queue
 */
pcb_t *resume(scheduler_t *scheduler);

/**
 * @brief Moves a PCB to Ready Suspended Queue
 *
 * @param scheduler the scheduler unit
 * @param pcb the pcb which was waiting for an IO event to finish
 */
void suspended_event(scheduler_t *scheduler, pcb_t *pcb);
