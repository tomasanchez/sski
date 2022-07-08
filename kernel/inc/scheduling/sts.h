/**
 * @file sts.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-03-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "scheduler.h"

void *short_term_schedule(void *data);

/**
 * @brief Restores a blocked PCB to the ready queue
 *
 * @param scheduler the Kernel Scheduler unit
 * @param pcb the unblocked PCB
 */
void event(scheduler_t *scheduler, pcb_t *pcb);

/**
 * @brief Blocks a PCB
 *
 * @param scheduler the Kernel Scheduler unit
 * @param pcb to be blocked
 * @param io_time the time to be blocked
 */
void block(scheduler_t *scheduler, pcb_t *pcb, uint32_t io_time);
