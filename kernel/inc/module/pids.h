#pragma once

#include <pthread.h>

#include "lib.h"

typedef struct pids
{
	pthread_mutex_t _mtx;
	bool pool[PIDS];
} pids_t;

pids_t new_pids();

void pids_destroy(pids_t *pids);

/**
 * @brief Get the primer pid libre
 *
 * @return el ID libre o UNDEFINED
 */
uint32_t get_pid_libre(pids_t *pids);
