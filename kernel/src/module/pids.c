#include "pids.h"

pids_t new_pids()
{
	pids_t pids;
	pthread_mutex_init(&(pids._mtx), NULL);

	for (int i = 0; i < PIDS; i++)
		pids.pool[i] = false;

	return pids;
}

void pids_destroy(pids_t *pids)
{
	pthread_mutex_destroy(&(pids->_mtx));
}

uint32_t get_pid_libre(pids_t *pids)
{
	uint32_t free_pid = UNDEFINED;
	pthread_mutex_lock(&(pids->_mtx));

	for (uint32_t i = 0; i < PIDS; i++)
	{
		if (!pids->pool[i])
		{
			pids->pool[i] = true;
			free_pid = i;
			break;
		}
	}
	pthread_mutex_unlock(&(pids->_mtx));

	return free_pid;
}

void pids_free(pids_t *pids, uint32_t pid)
{
	if (pid < PIDS)
	{
		pthread_mutex_lock(&(pids->_mtx));
		pids->pool[pid] = false;
		pthread_mutex_unlock(&(pids->_mtx));
	}
}
