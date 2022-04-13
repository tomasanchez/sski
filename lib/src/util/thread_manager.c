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

/**
 * @brief Manager de threads 
 */
typedef struct ThreadManager
{
	// Listado de Threads
	pthread_t *threads;
	// MUTEX de los Threads
	pthread_mutex_t mutex;
	// Cantidad Threads
	int size;
	// Cantidad maxima de threads
	int max_threads;
	// Si se inició o no
	bool init;
} thread_manager_t;

// El manager en sí
static thread_manager_t this;

// ============================================================================================================
//                               ***** Funciones Privadas - Declaraciones *****
// ============================================================================================================

/**
 * @brief Busca en la lista el índice del thread
 * 
 * @return el índice o ERROR 
 */
int conseguir_indice(void);

// ============================================================================================================
//                               ***** Funciones Privadas - Definiciones *****
// ============================================================================================================
int conseguir_indice(void)
{
	// El THREAD ID del thread llamante.
	pthread_t id = pthread_self();

	for (int i = 0; i < this.size; i++)
		if (this.threads[i] EQ id)
			return i;

	return ERROR;
}
// ============================================================================================================
//                               ***** Funciones Públicas - Definiciones *****
// ============================================================================================================

// ------------------------------------------------------------
//  Inicializador y Terminador
// ------------------------------------------------------------

int thread_manager_init(void)
{
	if (!this.init)
	{
		this.size = 0;
		this.max_threads = N_THREADS;
		this.threads = calloc(this.max_threads, sizeof(pthread_t));
		pthread_mutex_init(&this.mutex, NULL);
		this.init = true;
	}

	return SUCCESS;
}

void thread_manager_end(void)
{
	if (this.init)
	{
		for (int i = 0; i < this.size; i++)
		{
			pthread_kill(this.threads[i], SIGTERM);
		}

		pthread_mutex_destroy(&this.mutex);

		free(this.threads);
	}

	this.init = false;
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

void thread_manager_lanzar(void *(*rutina)(void *), void *__restrict argumentos)
{
	// Mientras haya lugar, agrego a la lista...
	// Necesita mutua exclusión
	THREAD_SAFE(
		if (this.size < this.max_threads)
		{
			pthread_create(&this.threads[this.size], NULL, rutina, argumentos);
			pthread_detach(this.threads[this.size]);
		} else
		{
			this.max_threads *= 2;
			this.threads = realloc(this.threads, this.max_threads * sizeof(*(this.threads)));
			pthread_create(&this.threads[this.size], NULL, rutina, argumentos);
			pthread_detach(this.threads[this.size]);
		} this.size++;);
}

void thread_manager_terminar_thread(void)
{
	// Se remueve de la lista, neceista mutua exclusión.
	THREAD_SAFE(
		int i = conseguir_indice();
		this.threads[i] = this.threads[this.size - 1];
		this.size--;);

	pthread_exit(NULL);
}

void thread_manager_terminar_thread_all(void)
{
	// Se remueve de la lista, neceista mutua exclusión.
	for (int i = 0; i < this.size; i++)
		pthread_kill(this.threads[i], SIGTERM);
}
