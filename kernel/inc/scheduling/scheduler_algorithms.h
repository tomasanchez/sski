/**
 * @file scheduler_algorithms.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-23-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "scheduler.h"

/**
 * @brief Get the next fifo object
 *
 * @param scheduler the Scheduler object
 * @return a PCB
 */
void *get_next_fifo(void *scheduler);

/**
 * @brief Get the next srt object
 *
 * @param scheduler the Scheduler object
 * @return a PCB
 */
void *get_next_srt(void *scheduler);
