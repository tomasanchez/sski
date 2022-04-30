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
	pthread_mutex_lock(&(pids->_mtx));
	for (uint32_t i = 0; i < PIDS; i++)
	{
		if (!pids->pool[i])
		{
			pids->pool[i] = true;
			return i;
		}
	}
	pthread_mutex_unlock(&(pids->_mtx));

	return UNDEFINED;
}
