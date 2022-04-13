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

void thread_manager_terminar_thread_all(void);
