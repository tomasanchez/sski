/**
 * thread_manager.c
 *
 * @file Manager de Threads
 * @author Tomás Sánchez
 * @since  05.08.2021
 */

#include "thread_manager.h"
#include <signal.h>
#include <commons/process.h>
#include <stdlib.h>

// ============================================================================================================
//                               ***** Estructuras y Definiciones *****
// ============================================================================================================

// El manager en sí
static thread_manager_t this;

// ============================================================================================================
//                               ***** Funciones Privadas - Declaraciones *****
// ============================================================================================================

/**
 * @brief Retreives the current thread index in a thread manager.
 *
 * @param tm the manager reference
 * @return the index or Error.
 */
static ssize_t thread_manager_get_index(const thread_manager_t *tm);
// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================

static ssize_t
thread_manager_get_index(const thread_manager_t *tm)
{
	// The calling thread ID (current thread).
	pthread_t id = pthread_self();

	for (ssize_t i = 0l; i < tm->size; i++)
		if (tm->threads[i] EQ id)
			return i;

	return ERROR;
}
// ============================================================================================================
//                               ***** Funciones Públicas - Definiciones *****
// ============================================================================================================

// ------------------------------------------------------------
//  Inicializador y Terminador
// ------------------------------------------------------------

thread_manager_t
new_thread_manager()
{
	thread_manager_t tm;

	tm.max_threads = N_THREADS;
	tm.threads = calloc(tm.max_threads, sizeof(pthread_t));
	pthread_mutex_init(&tm.mutex, NULL);
	tm.init = true;
	tm.size = 0;

	return tm;
}

void thread_manager_destroy(thread_manager_t *tm)
{
	if (tm->init)
	{
		thread_manager_end_threads(tm);
		pthread_mutex_destroy(&tm->mutex);
		free(tm->threads);
	}

	tm->threads = NULL;
	tm->size = 0;
	tm->init = false;
}

int thread_manager_init(void)
{
	if (!this.init)
	{
		this = new_thread_manager();
	}

	return SUCCESS;
}

void thread_manager_end(void)
{
	thread_manager_destroy(&this);
}

// ------------------------------------------------------------
//  Exclusión mutua
// ------------------------------------------------------------

inline void mutex_lock()
{
	if (this.init)
		pthread_mutex_lock(&this.mutex);
}

inline void mutex_unlock()
{
	if (this.init)
		pthread_mutex_unlock(&this.mutex);
}

// ------------------------------------------------------------
//  Agregar y Borrar thread
// ------------------------------------------------------------

void thread_manager_launch(thread_manager_t *tm,
						   void *(*thread_routine)(void *),
						   void *__restrict args)
{

	if (tm->init)
	{
		pthread_mutex_lock(&tm->mutex);

		if (tm->size < tm->max_threads)
		{
			pthread_create(&tm->threads[tm->size], NULL, thread_routine, args);
			pthread_detach(tm->threads[tm->size]);
		}
		else
		{
			tm->max_threads *= 2;
			tm->threads = realloc(tm->threads, tm->max_threads * sizeof(pthread_t));
			pthread_create(&tm->threads[tm->size], NULL, thread_routine, args);
			pthread_detach(tm->threads[tm->size]);
		}

		tm->size++;
		pthread_mutex_unlock(&tm->mutex);
	}
}

void thread_manager_lanzar(void *(*rutina)(void *), void *__restrict argumentos)
{
	thread_manager_launch(&this, rutina, argumentos);
}

int thread_manager_end_thread(thread_manager_t *tm)
{
	pthread_mutex_lock(&tm->mutex);
	// Pthread ID.
	ssize_t t_id = thread_manager_get_index(tm);

	// When no ID found fail.
	if (t_id == ERROR)
		return EXIT_FAILURE;

	// Move the last positition to the deleted one.
	if (tm->size > 1)
		tm->threads[t_id] = tm->threads[tm->size - 1];

	tm->size--;

	pthread_mutex_unlock(&tm->mutex);

	pthread_exit(NULL);

	return EXIT_SUCCESS;
}

void thread_manager_end_threads(thread_manager_t *tm)
{

	pthread_mutex_lock(&tm->mutex);

	for (ssize_t i = 0l; i < tm->size; i++)
	{
		pthread_cancel(tm->threads[i]);
		kill(tm->threads[i], SIGKILL);
	}

	tm->size = 0;
	pthread_mutex_unlock(&tm->mutex);
}

void thread_manager_terminar_thread(void)
{
	thread_manager_end_thread(&this);
}

void thread_manager_terminar_thread_all(void)
{
	thread_manager_end_threads(&this);
}
