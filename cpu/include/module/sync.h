/**
 * @file sync.h
 * @author Tomás Sánchez <tosanchez@frba.utn.edu.ar>
 * @brief
 * @version 0.1
 * @date 06-05-2022
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include "sem.h"

typedef struct CpuSync
{
	sem_t *pcb_received;
	sem_t *cpu_in_use;
} cpu_sync_t;

/**
 * @brief Initializes a Syncrhonizer.
 *
 * @return an initialized Syncrhonizer.
 */
cpu_sync_t init_sync();

/**
 * @brief Deallocates memory associated with a Syncrhonizer
 *
 * @param sync the Syncrhonizer to delete
 * @return cpu_sync_t
 */
void sync_destroy(cpu_sync_t *sync);
