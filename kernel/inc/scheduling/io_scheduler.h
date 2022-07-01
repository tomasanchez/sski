/**
 * @file io_scheduler.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 07-01-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

/**
 * @brief Given a Scheduler, executios IO burst of a PCB
 *
 * @param scheduler a Kernel Scheduler
 * @return null
 */
void *io_scheduler(void *scheduler);
