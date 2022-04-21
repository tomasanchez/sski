/**
 * thread_handler.h
 *
 * @file API de Threads
 * @author Tomás Sánchez
 * @since  05.08.2021
 */

#pragma once

#include "lib.h"
#include "log.h"
#include <pthread.h>

#define N_THREADS 6

/**
 * @brief Thread Launcher and Tracker.
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

// ============================================================================================================
//                               ***** Funciones Públicas - Definiciones *****
// ============================================================================================================

// ------------------------------------------------------------
//  Exclusión Mutua
// ------------------------------------------------------------

/**
 * @brief Realzia el lock del mutex del manager.
 */
void mutex_lock();

/**
 * @brief Realzia el unlock del mutex del manager.
 */
void mutex_unlock();

/**
 * @brief  Ejecuta un statement utilizando mutua exclusión.
 * @param statement un bloque a ejecutar
 */
#define THREAD_SAFE(function) \
	mutex_lock();             \
	function;                 \
	mutex_unlock()

// ------------------------------------------------------------
//  Inicializador y Terminador
// ------------------------------------------------------------

/**
 * @brief Creates a new thread manager ready to be used.
 *
 * @return The thread manager object copy.
 */
thread_manager_t new_thread_manager();

/**
 * @brief Frees the memory usage of a thread manager.
 *
 * @param tm the manager to be deleted.
 */
void thread_manager_destroy(thread_manager_t *tm);

/**
 * @brief Inicializa los recursos utilizados por el manager.
 *
 * @return SUCCESS o ERROR
 */
int thread_manager_init(void);

/**
 * @brief Realiza la liberación de recursos.
 */
void thread_manager_end(void);

// ------------------------------------------------------------
//  Lanzar y Terminar Threads
// ------------------------------------------------------------

/**
 * @brief Launchs a new pthread.
 *
 * @param tm a Thread Manager which tracks the thread.
 * @param thread_routine a procedure to be delegated to the new thread.
 * @param args data arguments
 */
void thread_manager_launch(thread_manager_t *tm,
						   void *(*thread_routine)(void *),
						   void *__restrict args);

/**
 * @brief Lanza un nuevo pthread
 *
 * @param rutina  la rutina a ejuctar
 * @param argumentos los argumentos que requiera
 */
void thread_manager_lanzar(void *(*rutina)(void *), void *__restrict argumentos);

/**
 * @brief Termina el thread que la ejecute. NO LLAMAR DESDE EL MAIN THREAD.
 */
void thread_manager_terminar_thread(void);

/**
 * @brief End all threads in the singleton thread manager.
 *
 */
void thread_manager_terminar_thread_all(void);

/**
 * @brief Ends the current thread.
 *
 * ! DO NOT CALL THIS METHOD FROM THE MAIN THREAD.
 *
 * @param tm the thread manager reference
 * @return wether success or fails
 */
int thread_manager_end_thread(thread_manager_t *tm);

/**
 * @brief End all the threads in a ThreadManager.
 *
 * @param tm Thread Manager to be emptied
 */
void thread_manager_end_threads(thread_manager_t *tm);
